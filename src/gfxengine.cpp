#include "gfxengine.hpp"
// #include "gui.hpp"
#include "util.hpp"
#include <stdexcept>

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

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    glCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatricies, 0, 2*sizeof(glm::mat4)));

    glCall(glGenBuffers(1, &m_uboLights));

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferData(GL_UNIFORM_BUFFER, m_uboLightsSize, NULL, GL_STATIC_DRAW));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    glCall(glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_uboLights, 0, m_uboLightsSize));

    // gui = new Gui(this);
}

GfxEngine::~GfxEngine()
{
    for (auto l : m_lights)
	delete l;

    // delete gui;

    if (s_instance == this) {
	s_instance = nullptr;
    }
}


void GfxEngine::SubmitLight(const Light &light)
{
    Light *newLight = new Light(light);

    if (m_lights.size() < m_maxLights)
	m_lights.push_back(newLight);
}

void GfxEngine::SubmitMesh(const Mesh &mesh, const Material &material, const glm::mat4 &transform)
{
    Renderable r = {mesh, material, transform};
    m_renderables.push_back(r);
}

void GfxEngine::SubmitModel(const std::vector<Mesh> &model, const Material &material, const glm::mat4 &transform)
{
    for (auto mesh : model)
    {
	Renderable r = {mesh, material, transform};
	m_renderables.push_back(r);
    }
}

void GfxEngine::BeginFrame(const Camera &camera)
{
    m_renderables.clear();
    m_lights.clear();

    glCall(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    // glfwGetFramebufferSize(m_window, &viewportWidth, &viewportHeight);

    glm::mat4 view = camera.GetViewMatrix();
    float aspectRatio = (float)m_screenWidth / (float)m_screenHeight;
    glm::mat4 projection = glm::perspective(
	glm::radians(camera.fov),
	aspectRatio, 0.1f, 100.0f);
    

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

}

void GfxEngine::EndFrame(const Camera &camera)
{
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));

    struct LightGPU {
	glm::vec4 position; // w is type (0 = DirLight, 1 = PointLight)
    	glm::vec4 ambient;
    	glm::vec4 diffuse;
    	glm::vec4 specular;
    };

    LightGPU ligtsGPU[m_maxLights]{};

    for (size_t i = 0; i < m_lights.size(); i++)
    {
	Light *light = m_lights[i];
	LightGPU lightGPU;
	lightGPU.position = glm::vec4(light->position.x, light->position.y, light->position.z, light->type);
	lightGPU.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, light->constant);
	lightGPU.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, light->linear);
	lightGPU.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, light->quadratic);

	ligtsGPU[i] = lightGPU;
    }

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_uboLightsSize, ligtsGPU));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    for (auto r : m_renderables)
    {

	glCall(glUseProgram(r.material.shader));

	SetMat4(r.material.shader, "model", glm::value_ptr(r.transform));
	glm::mat3 normalMat;
	normalMat = glm::transpose(glm::inverse(/* view * */ r.transform));
	SetMat3(r.material.shader, "normalMat", glm::value_ptr(normalMat));

	glCall(glUniform1i(glGetUniformLocation(r.material.shader, "numActiveLights"), m_lights.size()));
	glCall(glUniform3f(glGetUniformLocation(r.material.shader, "viewPos"),
		    camera.position.x, camera.position.y, camera.position.z));
	glCall(glUniform1f(glGetUniformLocation(r.material.shader, "material.shininess"), r.material.shininess));

	r.mesh.Draw(r.material.shader);
	
    }
}

size_t GfxEngine::GetLightCount()
{
    return m_lights.size();
}

Light* GfxEngine::GetLight(size_t index)
{
    if (index < m_lights.size())
	return m_lights[index];
    else
	throw std::exception();
}
