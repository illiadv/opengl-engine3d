#include "gfxengine.hpp"
#include "gui.hpp"
#include "util.hpp"
#include "glm/gtc/quaternion.hpp"
#include <stdexcept>

GfxEngine* GfxEngine::s_instance = nullptr;

GfxEngine::GfxEngine(int screenWidth, int screenHeight)
{
    if (s_instance != nullptr) {
	throw std::runtime_error("No more than one instance of GfxEngine is allowed");
    }
    s_instance = this;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(screenWidth, screenHeight, "Event Tester", NULL, NULL);

    if (m_window == NULL)
    {
	printf("Failed to init glfw window\n");
	glfwTerminate();
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	fprintf(stderr, "Failed to initialize GLAD\n");
    }

    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(m_window, &viewportWidth, &viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);

    stbi_set_flip_vertically_on_load(1);
    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &m_uboMatricies);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies);
    glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatricies, 0, 2*sizeof(glm::mat4));

    glGenBuffers(1, &m_uboLights);
    glCheckError();

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    glCheckError();
    glBufferData(GL_UNIFORM_BUFFER, m_uboLightsSize, NULL, GL_STATIC_DRAW);
    glCheckError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glCheckError();
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_uboLights, 0, m_uboLightsSize);
    glCheckError();

    m_normalShader = CreateShader("vertex.glsl", "normals.glsl");
    m_uvShader = CreateShader("vertex.glsl", "uvs.glsl");

    glfwSetFramebufferSizeCallback(m_window, s_framebuffer_size_callback);
    glfwSetKeyCallback(m_window, s_key_callback);
    glfwSetCursorPosCallback(m_window, s_mouse_callback);
    glfwSetMouseButtonCallback(m_window, s_mouse_button_callback);
    glfwSetScrollCallback(m_window, s_scroll_callback);

    gui = new Gui(this);
}

GfxEngine::~GfxEngine()
{
    for (auto o : m_objects)
	delete o;
    for (auto l : m_lights)
	delete l;

    delete gui;

    if (s_instance == this) {
	s_instance = nullptr;
    }
}

void GfxEngine::SetDefaultMaterial(Material* material)
{
    m_defaultMaterial = material;
}

void GfxEngine::SetHandleModel(Model* model)
{
    m_handleModel = model;
}

void GfxEngine::SetDebugShader(unsigned int shader)
{
    m_wireframeShader = shader;
}

void GfxEngine::SetCamera(Camera *camera)
{
    m_camera = camera;
}

void GfxEngine::AddLight(Light light)
{
    Light *newLight = new Light(light);

    if (m_lights.size() < m_maxLights)
	m_lights.push_back(newLight);
}

void GfxEngine::Draw() {

    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(m_window, &viewportWidth, &viewportHeight);

    glm::mat4 view = m_camera->GetViewMatrix();
	Material* thisMaterial;
    glm::mat4 projection = glm::perspective(
	    glm::radians(m_camera->fov),
	    (float)viewportWidth / (float)viewportHeight, 0.1f, 100.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies);
    glCheckError();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glCheckError();
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glCheckError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glCheckError();

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    glCheckError();

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

    glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
    glCheckError();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, m_uboLightsSize, ligtsGPU);
    glCheckError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glCheckError();


    for (Object *object : m_objects)
    {

	if (object->material == nullptr)
	{
	    thisMaterial = m_defaultMaterial;
	}
	else
	{
	    thisMaterial = object->material;
	}

	// TODO: Do smth about this
	glUseProgram(thisMaterial->shader);
	// light->UpdateUniforms(thisMaterial->shader);

	glCheckError();

	glUniform1i(glGetUniformLocation(thisMaterial->shader, "numActiveLights"), m_lights.size());
	glCheckError();

	glUniform3f(glGetUniformLocation(thisMaterial->shader, "viewPos"),
		    m_camera->position.x, m_camera->position.y, m_camera->position.z);
	glCheckError();

	glUniform1f(glGetUniformLocation(thisMaterial->shader, "material.shininess"), thisMaterial->shininess);
	glCheckError();

	if (debugDrawMode == 2) {
	    glUseProgram(m_normalShader);
	    object->Draw(m_normalShader);
	}
	else if (debugDrawMode == 3) {
	    glUseProgram(m_uvShader);
	    object->Draw(m_uvShader);
	}
	else {
	    object->Draw(thisMaterial->shader);
	}
	
	    glCheckError();
	
	// Debug draw
	if (debugDrawWireframes) {
	    glUseProgram(m_wireframeShader);
	    SetVec3(m_wireframeShader, "color", 1.0f, 0.4f, 0.0f);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	    object->Draw(m_wireframeShader);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	    glCheckError();
	}
    }

    if (debugDrawLightHandles) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(m_wireframeShader);
	SetVec3(m_wireframeShader, "color", 1.0f, 0.8f, 0.0f);
	for (auto light : m_lights)
	{
	    glm::vec3 scale(0.1f);
	    glm::vec3 position = light->position;
	    glm::vec3 direction({0.0f, 0.0f, 1.0f});

	    if (light->type != 1.0f) {
		position = glm::vec3(0, 10, 0);
		direction = light->position;
	    }
	    DrawHandle(m_handleModel, position, scale, direction);

	}
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (debugDrawObjectHandles) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(m_wireframeShader);
	SetVec3(m_wireframeShader, "color", 0.0f, 1.0f, 1.0f);
	for (auto object : m_objects)
	{
	    DrawHandle(m_handleModel, object->position, glm::vec3(0.01f));
	}
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    gui->Draw();

}


void GfxEngine::DrawHandle(Model* model, glm::vec3 position, glm::vec3 scale, glm::vec3 direciton)
{
	glUseProgram(m_wireframeShader);
	glCheckError();

	glCheckError();

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, position);
	modelMat = modelMat * glm::mat4_cast(glm::quatLookAt(direciton, glm::vec3(0, 1, 0)));
	(void)direciton;

	float distance = glm::length(m_camera->position - position);
	float fovCorrection = tan(glm::radians(m_camera->fov) / 2);
	glm::vec3 finalScale = scale * fovCorrection * distance;
	modelMat = glm::scale(modelMat, finalScale);

	SetMat4(m_wireframeShader, "model", glm::value_ptr(modelMat));

	// set normal mat
	
	glCheckError();
	model->Draw(m_wireframeShader);
}


GLFWwindow *GfxEngine::GetWindow()
{
    return m_window;
}

Camera *GfxEngine::GetCamera()
{
    return m_camera;
}


Object *GfxEngine::AddObject(BaseModel* model)
{
    Object* object = new Object(model);
    m_objects.push_back(object);
    return object;
}

size_t GfxEngine::GetObjectCount()
{
    return m_objects.size();
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

Object* GfxEngine::GetObject(size_t index)
{
    if (index < m_objects.size())
	return m_objects[index];
    else
	throw std::exception();
}

void GfxEngine::RemoveObject(Object *object) {
    for (size_t i = 0; i < m_objects.size(); i++)
    {
	if (m_objects[i] == object) {
	    delete m_objects[i];
	    m_objects.erase(m_objects.begin() + i);
	}
    }
}

void GfxEngine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
} 

void GfxEngine::processInput(float deltaTime)
{
    if (gui->WantKeyboard()) {
	return;
    }

    if(glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
	glfwSetWindowShouldClose(m_window, GLFW_TRUE);

    if(glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS)
	glfwSetWindowShouldClose(m_window, GLFW_TRUE);

    if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	m_camera->ProcessMovement(FORWARD, deltaTime);
    if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	m_camera->ProcessMovement(BACKWARD, deltaTime);
    if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	m_camera->ProcessMovement(LEFT, deltaTime);
    if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	m_camera->ProcessMovement(RIGHT, deltaTime);
    if(glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	m_camera->ProcessMovement(UP, deltaTime);
    if(glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	m_camera->ProcessMovement(DOWN, deltaTime);

}

void GfxEngine::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (gui->WantKeyboard())
	return;

    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}


void GfxEngine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (gui->WantMouse())
	return;

    (void)window;

    float xOffset = xpos - mouseLastX;
    float yOffset = mouseLastY - ypos;
    mouseLastX = xpos;
    mouseLastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	m_camera->ProcessLookAround(xOffset, yOffset);
}

void GfxEngine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (gui->WantMouse())
    {
	return;
    }
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void GfxEngine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (gui->WantMouse())
	return;

    (void)window;
    (void)xoffset;

    m_camera->ProcessZoom((float)yoffset);
}

void GfxEngine::s_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (s_instance) {
	s_instance->framebuffer_size_callback(window, width, height);
    }
}

void GfxEngine::s_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (s_instance) {
	s_instance->key_callback(window, key, scancode, action, mods);
    }
}

void GfxEngine::s_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{   
    if (s_instance) {
	s_instance->mouse_callback(window, xpos, ypos);
    }
}

void GfxEngine::s_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{   
    if (s_instance) {
	s_instance->mouse_button_callback(window, button, action, mods);
    }
}
void GfxEngine::s_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{   
    if (s_instance) {
	s_instance->scroll_callback(window, xoffset, yoffset);
    }
}
