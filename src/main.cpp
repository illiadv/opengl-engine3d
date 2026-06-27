#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gfxengine.hpp"

GLFWwindow *window;
Camera camera;

const int screenWidth = 1280;
const int screenHeight = 720;

float mouseLastX = 400;
float mouseLastY = 300;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    (void)window;

    float xOffset = xpos - mouseLastX;
    float yOffset = mouseLastY - ypos;
    mouseLastX = xpos;
    mouseLastY = ypos;

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	camera.ProcessLookAround(xOffset, yOffset);

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

void InitWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "engine3d", NULL, NULL);

    if (window == NULL)
    {
	printf("Failed to init glfw window\n");
	glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	fprintf(stderr, "Failed to initialize GLAD\n");
    }

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // int viewportWidth, viewportHeight;
    // glfwGetFramebufferSize(window, &viewportWidth, &viewportHeight);
}

int main()
{
    InitWindow();
    GfxEngine engine(1280, 720);


    unsigned int shaderProgram = CreateShader("vertex.glsl", "fragment.glsl");
    unsigned int shaderProgramGrass = CreateShader("vertex.glsl", "grass.glsl");

    unsigned int shaderProgramSingleColor = CreateShader("vertex.glsl", "singleColor.glsl");

    Material materialDefault = Material(shaderProgram, 64);
    // Material materialGrass = Material(shaderProgramGrass, 0);
    // Material materialSingleColor = Material(shaderProgramSingleColor, 0);

    auto treeModel = ModelLoader::Load("assets/laubbaum/laubbaum.obj");
    auto catModel = ModelLoader::Load("assets/cat/cat.obj");
    auto crateModel = ModelLoader::Load("assets/crate/crate.obj");
    auto groundModel = ModelLoader::Load("assets/ground/ground.obj");
    auto girlModel = ModelLoader::Load("assets/pink/pink.obj");
    auto grassModel = ModelLoader::Load("assets/grass/grass.obj");
    auto cylinderModel = ModelLoader::Load("assets/cylinder/cylinder.obj");
    auto lightHandleModel = ModelLoader::Load("assets/light-handle/light-handle.obj");

    // VoxelModel voxelmodel("vmodel.txt");


    // Object* backpack = engine.AddObject(&backpackModel);
    //
    // backpack->SetPosition(glm::vec3(2.0, 1.0, 1.0f));
    // backpack->SetScale(glm::vec3(0.3f));



    Light dirLight(glm::vec3(-0.2f, -0.5f, -1.0f), glm::vec3(0.2f), glm::vec3(0.7f), glm::vec3(0.5f));
    Light pointLight(glm::vec3(0.0f, 3.0f, 3.0f), glm::vec3(1.0f), glm::vec3(1.0f, 0.8f, 0.0f), glm::vec3(0.5f), 1.0f, 0.09f, 0.032f);
    
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!glfwWindowShouldClose(window))
    {
	processInput(deltaTime);

	engine.BeginFrame(camera);


	engine.SubmitModel(crateModel, materialDefault, glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 6)));
	engine.SubmitModel(catModel, materialDefault, glm::mat4(1.0f));
	engine.SubmitModel(treeModel, materialDefault, glm::translate(glm::mat4(1.0f), glm::vec3(2, 0, 6)));
	engine.SubmitModel(groundModel, materialDefault, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0)), glm::vec3(10.0f)));

	engine.SubmitModel(girlModel, materialDefault, glm::scale(
		    glm::rotate(
		    glm::translate(glm::mat4(1.0f), glm::vec3(2.5, 0, 0)),
		    glm::radians(105.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
		    glm::vec3(2.0f)));

	engine.SubmitLight(dirLight);
	engine.SubmitLight(pointLight);


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
	

	engine.EndFrame(camera);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;  

	glfwSwapBuffers(window);
	glfwPollEvents();
    }
	    
    // clean up

    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgramGrass);
    glfwTerminate();
    printf("Bye\n");
    
    return 0;
}
