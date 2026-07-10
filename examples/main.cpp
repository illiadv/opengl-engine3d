#include <stdio.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine3d/camera.hpp"
#include "engine3d/gfxengine.hpp"
#include "engine3d/light.hpp"
#include "engine3d/model.hpp"

GLFWwindow *window;
Camera camera;

const int screenWidth = 1280;
const int screenHeight = 720;

float mouseLastX = 400;
float mouseLastY = 300;

void MouseCallback(GLFWwindow *window, double xpos, double ypos)
{

    (void)window;

    float xOffset = xpos - mouseLastX;
    float yOffset = mouseLastY - ypos;
    mouseLastX = xpos;
    mouseLastY = ypos;

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	camera.ProcessLookAround(xOffset, yOffset);
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    void *pointer = glfwGetWindowUserPointer(window);
    GfxEngine *engine = static_cast<GfxEngine*>(pointer);
    engine->ResizeViewport(width, height);
}

void processInput(float deltaTime)
{
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GLFW_TRUE);

    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GLFW_TRUE);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	camera.ProcessMovement(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	camera.ProcessMovement(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	camera.ProcessMovement(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	camera.ProcessMovement(RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	camera.ProcessMovement(UP, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	camera.ProcessMovement(DOWN, deltaTime);

}

bool InitWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "engine3d", NULL, NULL);

    if (window == NULL)
    {
	printf("Failed to initilize glfw window\n");
	return false;
    }

    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void SubmitModel(GfxEngine &engine, const Model &model, const glm::mat4 &transform)
{
    for (size_t i = 0; i < model.meshes.size(); i++)
    {
	int materialIndex = model.materialIndexes[i];
	engine.SubmitMesh(&model.meshes[i], &model.materials[materialIndex], transform);
    }
}

int main()
{
    if (!InitWindow())
	return 1;
    GfxEngine engine(1280, 720, glfwGetProcAddress);
    glfwSetWindowUserPointer(window, &engine);
    int viewportWidth, viewportHeight;
    glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
    engine.ResizeViewport(viewportWidth, viewportHeight);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    auto shader = std::make_shared<Shader>(
	    "assets/shaders/vertex.glsl",
	    "assets/shaders/fragment.glsl");

    Model treeModel("assets/laubbaum/laubbaum.obj", shader);
    Model catModel("assets/cat/cat.obj", shader);
    Model crateModel("assets/crate/crate.obj", shader);
    Model groundModel("assets/ground/ground.obj", shader);
    Model girlModel("assets/pink/pink.obj", shader);
    Model grassModel("assets/grass/grass.obj", shader);
    Model cylinderModel("assets/cylinder/cylinder.obj", shader);
    Model lightHandleModel("assets/light-handle/light-handle.obj", shader);

    Model cubeballModel("assets/cube-ball/cube-ball.obj", shader);

    DirectionalLight dirLight(glm::vec3(-0.2f, -0.5f, -1.0f), glm::vec3(0.2f), glm::vec3(0.7f), glm::vec3(0.5f));
    PointLight pointLight(glm::vec3(0.0f, 5.0f, 3.0f), glm::vec3(1.0f), glm::vec3(1.0f, 0.8f, 0.0f), glm::vec3(0.5f), 50);
    
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!glfwWindowShouldClose(window))
    {
	processInput(deltaTime);

	engine.BeginFrame(camera);

	SubmitModel(engine, crateModel, glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 6)));
	SubmitModel(engine, catModel, glm::mat4(1.0f));
	SubmitModel(engine, cubeballModel, glm::translate(glm::mat4(1.0f), glm::vec3(-2, 0, -8)));
	SubmitModel(engine, treeModel, glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 6)));
	SubmitModel(engine, groundModel, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0)), glm::vec3(10.0f)));
	SubmitModel(engine, girlModel, glm::scale(
		    glm::rotate(
		    glm::translate(glm::mat4(1.0f), glm::vec3(2.5, 0, 0)),
		    glm::radians(195.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		    glm::vec3(2.0f)));

	engine.SubmitLight(&dirLight);
	engine.SubmitLight(&pointLight);


	// for (int i = 0; i < 20; i++)
	// {
	//     float x = (float)(rand() % 100) / 10.0f - 5.0f;
	//     float z = (float)(rand() % 100) / 10.0f - 5.0f;
	//     glm::vec3 pos = glm::vec3(x * 1.5, 0.0f, z * 1.5);
	//
	//     Object* grass = engine.AddObject(&grassModel);
	//     grass->SetPosition(pos);
	//     grass->Rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//     grass->SetMaterial(&materialDefault);
	// }
	

	engine.EndFrame();

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;  

	glfwSwapBuffers(window);
	glfwPollEvents();
    }
	    
    // clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    printf("Bye\n");
    
    return 0;
}
