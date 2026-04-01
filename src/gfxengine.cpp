#include "gfxengine.hpp"
#include "util.hpp"
#include "glm/gtc/quaternion.hpp"


GfxEngine::GfxEngine(int screenWidth, int screenHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "Event Tester", NULL, NULL);

    if (window == NULL)
    {
	printf("Failed to init glfw window\n");
	glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	fprintf(stderr, "Failed to initialize GLAD\n");
    }

    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
    glViewport(0, 0, viewportWidth, viewportHeight);

    stbi_set_flip_vertically_on_load(1);
    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &uboMatricies);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
    glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatricies, 0, 2*sizeof(glm::mat4));

    glGenBuffers(1, &uboLights);
    glCheckError();

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glCheckError();
    glBufferData(GL_UNIFORM_BUFFER, uboLightsSize, NULL, GL_STATIC_DRAW);
    glCheckError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glCheckError();
    glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLights, 0, uboLightsSize);
    glCheckError();

    normalShader = CreateShader("vertex.glsl", "normals.glsl");
    uvShader = CreateShader("vertex.glsl", "uvs.glsl");
}

GfxEngine::~GfxEngine()
{
    for (auto o : objects)
	delete o;
    for (auto l : lights)
	delete l;
}

void GfxEngine::SetDefaultMaterial(Material* material)
{
    defaultMaterial = material;
}

void GfxEngine::SetHandleModel(Model* model)
{
    handleModel = model;
}

void GfxEngine::SetDebugShader(unsigned int shader)
{
    wireframeShader = shader;
}

void GfxEngine::SetActiveCamera(Camera *camera)
{
    activeCamera = camera;
}

void GfxEngine::AddLight(Light light)
{
    Light *newLight = new Light(light);

    if (lights.size() < maxLights)
	lights.push_back(newLight);
}

void GfxEngine::Draw() {

    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);

    glm::mat4 view = activeCamera->GetViewMatrix();
	Material* thisMaterial;
    glm::mat4 projection = glm::perspective(
	    glm::radians(activeCamera->fov),
	    (float)viewportWidth / (float)viewportHeight, 0.1f, 100.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatricies);
    glCheckError();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glCheckError();
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glCheckError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glCheckError();

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glCheckError();

    struct LightGPU {
	glm::vec4 position; // w is type (0 = DirLight, 1 = PointLight)
    	glm::vec4 ambient;
    	glm::vec4 diffuse;
    	glm::vec4 specular;
    };

    LightGPU ligtsGPU[maxLights]{};

    for (size_t i = 0; i < lights.size(); i++)
    {
	Light *light = lights[i];
	LightGPU lightGPU;
	lightGPU.position = glm::vec4(light->position.x, light->position.y, light->position.z, light->type);
	lightGPU.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, light->constant);
	lightGPU.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, light->linear);
	lightGPU.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, light->quadratic);

	ligtsGPU[i] = lightGPU;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glCheckError();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, uboLightsSize, ligtsGPU);
    glCheckError();
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glCheckError();


    for (Object *object : objects)
    {

	if (object->material == nullptr)
	{
	    thisMaterial = defaultMaterial;
	}
	else
	{
	    thisMaterial = object->material;
	}

	// TODO: Do smth about this
	glUseProgram(thisMaterial->shader);
	// light->UpdateUniforms(thisMaterial->shader);

	glCheckError();

	glUniform1i(glGetUniformLocation(thisMaterial->shader, "numActiveLights"), lights.size());
	glCheckError();

	glUniform3f(glGetUniformLocation(thisMaterial->shader, "viewPos"),
		    activeCamera->position.x, activeCamera->position.y, activeCamera->position.z);
	glCheckError();

	glUniform1f(glGetUniformLocation(thisMaterial->shader, "material.shininess"), thisMaterial->shininess);
	glCheckError();

	if (debugDrawMode == 2) {
	    glUseProgram(normalShader);
	    object->Draw(normalShader);
	}
	else if (debugDrawMode == 3) {
	    glUseProgram(uvShader);
	    object->Draw(uvShader);
	}
	else {
	    object->Draw(thisMaterial->shader);
	}
	
	    glCheckError();
	
	// Debug draw
	if (debugDrawWireframes) {
	    glUseProgram(wireframeShader);
	    SetVec3(wireframeShader, "color", 1.0f, 0.4f, 0.0f);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	    object->Draw(wireframeShader);
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	    glCheckError();
	}
    }

    if (debugDrawLightHandles) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(wireframeShader);
	SetVec3(wireframeShader, "color", 1.0f, 0.8f, 0.0f);
	for (auto light : lights)
	{
	    glm::vec3 scale(0.1f);
	    glm::vec3 position = light->position;
	    glm::vec3 direction({0.0f, 0.0f, 1.0f});

	    if (light->type != 1.0f) {
		position = glm::vec3(0, 10, 0);
		direction = light->position;
	    }
	    DrawHandle(handleModel, position, scale, direction);

	}
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (debugDrawObjectHandles) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(wireframeShader);
	SetVec3(wireframeShader, "color", 0.0f, 1.0f, 1.0f);
	for (auto object : objects)
	{
	    DrawHandle(handleModel, object->position, glm::vec3(0.01f));
	}
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


void GfxEngine::DrawHandle(Model* model, glm::vec3 position, glm::vec3 scale, glm::vec3 direciton)
{
	glUseProgram(wireframeShader);
	glCheckError();

	glCheckError();

	glm::mat4 modelMat = glm::mat4(1.0f);
	modelMat = glm::translate(modelMat, position);
	modelMat = modelMat * glm::mat4_cast(glm::quatLookAt(direciton, glm::vec3(0, 1, 0)));
	(void)direciton;

	float distance = glm::length(activeCamera->position - position);
	float fovCorrection = tan(glm::radians(activeCamera->fov) / 2);
	glm::vec3 finalScale = scale * fovCorrection * distance;
	modelMat = glm::scale(modelMat, finalScale);

	SetMat4(wireframeShader, "model", glm::value_ptr(modelMat));

	// set normal mat
	
	glCheckError();
	model->Draw(wireframeShader);
}


GLFWwindow *GfxEngine::GetWindow()
{
    return window;
}


Object *GfxEngine::AddObject(BaseModel* model)
{
    Object* object = new Object(model);
    objects.push_back(object);
    return object;
}

size_t GfxEngine::GetObjectCount()
{
    return objects.size();
}

size_t GfxEngine::GetLightCount()
{
    return lights.size();
}

Light* GfxEngine::GetLight(size_t index)
{
    if (index < lights.size())
	return lights[index];
    else
	throw std::exception();
}

Object* GfxEngine::GetObject(size_t index)
{
    if (index < objects.size())
	return objects[index];
    else
	throw std::exception();
}

void GfxEngine::RemoveObject(Object *object) {
    for (size_t i = 0; i < objects.size(); i++)
    {
	if (objects[i] == object) {
	    delete objects[i];
	    objects.erase(objects.begin() + i);
	}
    }
}
