#include <algorithm>
#include <stdexcept>

#include "glad/gl.h"
#include "engine3d/gfxengine.hpp"
#include "engine3d/camera.hpp"
#include "engine3d/light.hpp"
#include "engine3d/mesh.hpp"
#include "engine3d/material.hpp"
#include "util.hpp"
#include "stb_image.h"


struct RenderCommand
{
    uint32_t sortKey;
    const Mesh *mesh;
    const Material *material;
    glm::mat4 transform;

    RenderCommand(const Mesh *mesh, const Material *material, glm::mat4 transform)
	: mesh(mesh), material(material), transform(transform)
    {
	sortKey = material->GetShader()->GetID() << 24 |
	    material->GetID() | 16;
	    mesh->GetID();
    }
};


GfxEngine* GfxEngine::s_instance = nullptr;

GfxEngine::GfxEngine(int screenWidth, int screenHeight, void (*(*loadFunc)(const char *))())
{
    if (s_instance != nullptr) {
	throw std::runtime_error("No more than one instance of GfxEngine is allowed");
    }
    s_instance = this;

    int version = gladLoadGL(loadFunc);
    if (version == 0)
    {
	throw std::runtime_error("Failed to initialize OpenGL context");
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    if (GLAD_VERSION_MAJOR(version) < 3 || (GLAD_VERSION_MAJOR(version) == 3 && GLAD_VERSION_MINOR(version) < 3))
    {
	throw std::runtime_error("Engine Error: The Host Application did not create an OpenGL 3.3+ context!");
    }


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
    RenderCommand r(mesh, material, transform);
    m_renderQueue.push_back(r);
}

void GfxEngine::BeginFrame(const Camera &camera)
{
    m_renderQueue.clear();
    m_lights.clear();

    glCall(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix(m_screenWidth, m_screenHeight);

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.position)));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

}

void GfxEngine::LightConverter::ConsumeDirectionalLight(const DirectionalLight* light)
{
    result = LightGPU {
	.position = glm::vec4(light->direction.x, light->direction.y, light->direction.z, 0.0f),
	.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, 0.0f),
	.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, 0.0f),
	.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, 0.0f),
    };
}

void GfxEngine::LightConverter::ConsumePointLight(const PointLight* light)
{
    result = LightGPU {
	.position = glm::vec4(light->position.x, light->position.y, light->position.z, 1.0f),
	.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, light->constant),
	.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, light->linear),
	.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, light->quadratic),
    };
}

void GfxEngine::EndFrame()
{
    LightGPU ligtsGPU[m_maxLights]{};

    for (size_t i = 0; i < m_lights.size(); i++)
    {
	LightConverter converter;
	const Light *light = m_lights[i];
	light->Accept(converter);
	ligtsGPU[i] = converter.result;
    }

    unsigned int numActiveLigths = m_lights.size();

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_lightsArraySize, ligtsGPU));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, m_lightsArraySize, sizeof(unsigned int), &numActiveLigths));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    auto queueSorter = [](const RenderCommand &a, const RenderCommand &b)
    {
	return a.sortKey < b.sortKey;
    };

    std::sort(m_renderQueue.begin(), m_renderQueue.end(), queueSorter);

    unsigned int boundShader = 0;
    unsigned int boundMaterial = 0;
    unsigned int boundMesh = 0;

    for (auto r : m_renderQueue)
    {
	glm::mat3 normalMat;
	normalMat = glm::transpose(glm::inverse(/* view * */ r.transform));

	unsigned int shaderID = r.material->GetShader()->GetID();
	unsigned int materialID = r.material->GetID();
	unsigned int meshID = r.mesh->GetID();

	if (shaderID != boundShader)
	{
	    r.material->GetShader()->Bind();
	    boundShader = shaderID;
	}

	if (materialID != boundMaterial)
	{
	    r.material->Bind();
	    boundMaterial = materialID;
	}

	r.material->GetShader()->SetMat4("model", glm::value_ptr(r.transform));
	r.material->GetShader()->SetMat3("normalMat", glm::value_ptr(normalMat));

	if (meshID != boundMesh)
	{
	    r.mesh->Bind();
	    boundMesh = meshID;
	}
	r.mesh->Draw();
    }
}

int GfxEngine::GetScreenWidth() const
{
    return m_screenWidth;
}

int GfxEngine::GetScreenHeight() const
{
    return m_screenHeight;
}
