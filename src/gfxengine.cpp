#include "gfxengine.hpp"
#include "util.hpp"


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

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatricies, 0, 2*sizeof(glm::mat4));



    // glEnable(GL_STENCIL_TEST);
}

GfxEngine::~GfxEngine()
{
    for (auto o : objects)
	delete o;
}

void GfxEngine::SetDefaultMaterial(Material* material)
{
    defaultMaterial = material;
}

void GfxEngine::SetActiveCamera(Camera *camera)
{
    activeCamera = camera;
}

void GfxEngine::SetLight(Light *light)
{
    this->light = light;
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
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
	light->UpdateUniforms(thisMaterial->shader);

	glCheckError();

	glUniform3f(glGetUniformLocation(thisMaterial->shader, "viewPos"),
		    activeCamera->position.x, activeCamera->position.y, activeCamera->position.z);
	glCheckError();

	glUniform1f(glGetUniformLocation(thisMaterial->shader, "material.shininess"), thisMaterial->shininess);
	glCheckError();

	object->Draw(thisMaterial->shader);
	glCheckError();
    }
}

GLFWwindow *GfxEngine::GetWindow()
{
    return window;
}


Object *GfxEngine::AddObject(Model* model)
{
    Object* object = new Object(model);
    objects.push_back(object);
    return object;
}

size_t GfxEngine::GetObjectCount()
{
    return objects.size();
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
