#include <stdexcept>

#include "engine3d/gfxengine.hpp"
#include "engine3d/camera.hpp"
#include "engine3d/light.hpp"
#include "engine3d/mesh.hpp"
#include "engine3d/material.hpp"
#include "util.hpp"
#include "stb_image.h"


struct Renderable
{
    const Mesh *mesh;
    const Material *material;
    glm::mat4 transform;
};


GfxEngine* GfxEngine::s_instance = nullptr;

GfxEngine::GfxEngine(int screenWidth, int screenHeight)
{
    if (s_instance != nullptr) {
	throw std::runtime_error("No more than one instance of GfxEngine is allowed");
    }
    s_instance = this;

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    glViewport(0, 0, screenWidth, screenHeight);

    stbi_set_flip_vertically_on_load(1);
    glEnable(GL_DEPTH_TEST);

    glCall(glGenBuffers(1, &m_uboMatricies));

    constexpr unsigned int uboMatriciesSize =
	2 * sizeof(glm::mat4)
	+ sizeof (glm::vec3);

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferData(GL_UNIFORM_BUFFER, uboMatriciesSize, NULL, GL_STATIC_DRAW));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    glCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatricies, 0, uboMatriciesSize));

    glCall(glGenBuffers(1, &m_uboLights));

    constexpr unsigned int uboLigthsSize =
	m_lightsArraySize
	+ sizeof(unsigned int);

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferData(GL_UNIFORM_BUFFER, uboLigthsSize, NULL, GL_STATIC_DRAW));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    glCall(glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_uboLights, 0, uboLigthsSize));

    // gui = new Gui(this);
}

void GfxEngine::ResizeViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

GfxEngine::~GfxEngine()
{
    if (s_instance == this) {
	s_instance = nullptr;
    }
}


void GfxEngine::SubmitLight(const Light *light)
{
    if (m_lights.size() < m_maxLights)
	m_lights.push_back(light);
}

void GfxEngine::SubmitMesh(const Mesh *mesh, const Material *material, const glm::mat4 &transform)
{
    Renderable r = {mesh, material, transform};
    m_renderables.push_back(r);
}

void GfxEngine::BeginFrame(const Camera &camera)
{
    m_renderables.clear();
    m_lights.clear();

    glCall(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    glm::mat4 view = camera.GetViewMatrix();
    float aspectRatio = (float)m_screenWidth / (float)m_screenHeight;
    glm::mat4 projection = glm::perspective(
	glm::radians(camera.fov),
	aspectRatio, 0.1f, 100.0f);
    

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.position)));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

}

void GfxEngine::EndFrame()
{
    struct LightGPU {
	glm::vec4 position; // w is type (0 = DirLight, 1 = PointLight)
    	glm::vec4 ambient;
    	glm::vec4 diffuse;
    	glm::vec4 specular;
    };

    LightGPU ligtsGPU[m_maxLights]{};

    for (size_t i = 0; i < m_lights.size(); i++)
    {
	const Light *light = m_lights[i];
	LightGPU lightGPU;
	lightGPU.position = glm::vec4(light->position.x, light->position.y, light->position.z, light->type);
	lightGPU.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, light->constant);
	lightGPU.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, light->linear);
	lightGPU.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, light->quadratic);

	ligtsGPU[i] = lightGPU;
    }

    unsigned int numActiveLigths = m_lights.size();

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_lightsArraySize, ligtsGPU));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, m_lightsArraySize, sizeof(unsigned int), &numActiveLigths));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    for (auto r : m_renderables)
    {
	glm::mat3 normalMat;
	normalMat = glm::transpose(glm::inverse(/* view * */ r.transform));

	r.material->Bind();

	r.material->GetShader()->SetMat4("model", glm::value_ptr(r.transform));
	r.material->GetShader()->SetMat3("normalMat", glm::value_ptr(normalMat));

	r.mesh->Draw();
	
	r.material->Unbind();
    }
}
