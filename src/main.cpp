#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gfxengine.hpp"

#include "voxelmodel.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void ImguiInit(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
} 

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Camera camera;

bool guiActive = false;

void processInput(GLFWwindow *window)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
	return;

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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (ImGui::GetIO().WantCaptureKeyboard)
	return;

    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

float mouseLastX = 400;
float mouseLastY = 300;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (ImGui::GetIO().WantCaptureMouse)
	return;

    (void)window;

    float xOffset = xpos - mouseLastX;
    float yOffset = mouseLastY - ypos;
    mouseLastX = xpos;
    mouseLastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	camera.ProcessLookAround(xOffset, yOffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
	return;
    }
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (ImGui::GetIO().WantCaptureMouse)
	return;

    (void)window;
    (void)xoffset;

    camera.ProcessZoom((float)yoffset);
}

void Drag3Rotation(const char* label, Object* object)
{

    if (ImGui::DragFloat3(label, glm::value_ptr(object->eulerRotation), 0.1f))
    {
	glm::quat q = glm::quat(glm::radians(object->eulerRotation));
	object->SetRotation(q);
    }



}

int main()
{
    GfxEngine engine(1000, 600);
    // glfwSetInputMode(engine.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_ENA);
    glfwSetFramebufferSizeCallback(engine.GetWindow(), framebuffer_size_callback);  
    glfwSetKeyCallback(engine.GetWindow(), key_callback);
    glfwSetCursorPosCallback(engine.GetWindow(), mouse_callback);  
    glfwSetMouseButtonCallback(engine.GetWindow(), mouse_button_callback);  
    glfwSetScrollCallback(engine.GetWindow(), scroll_callback);  

    ImguiInit(engine.GetWindow());


    unsigned int shaderProgram = CreateShader("vertex.glsl", "fragment.glsl");
    unsigned int shaderProgramGrass = CreateShader("vertex.glsl", "grass.glsl");

    unsigned int shaderProgramSingleColor = CreateShader("vertex.glsl", "singleColor.glsl");

    Material materialDefault = Material(shaderProgram, 64);
    Material materialGrass = Material(shaderProgramGrass, 0);
    Material materialSingleColor = Material(shaderProgramSingleColor, 0);

    Model backpackModel("assets/backpack/backpack.obj", true);
    Model catModel("assets/cat/cat.obj");
    Model crateModel("assets/crate/crate.obj");
    Model groundModel("assets/ground/ground.obj");
    Model girlModel("assets/pickme/pickme.obj");
    Model grassModel("assets/grass/grass.obj");

    Model cylinderModel("assets/cylinder/cylinder.obj");

    VoxelModel voxelmodel("vmodel.txt");

    // Object* backpack = engine.AddObject(&backpackModel);
    //
    // backpack->SetPosition(glm::vec3(2.0, 1.0, 1.0f));
    // backpack->SetScale(glm::vec3(0.3f));



    Object *crate = engine.AddObject(&crateModel);
    // crate->SetScale(glm::vec3(0.1f, 0.1f, 1.0f));
    crate->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    Object *cat = engine.AddObject(&catModel);
    (void)cat;
    // cat->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
    
    Object *voxels = engine.AddObject(&voxelmodel);
    voxels->SetPosition(glm::vec3(0.0f, 6.0f, 0.0f));
    // voxels->SetMaterial(&materialSingleColor);

    Object *ground = engine.AddObject(&groundModel);
    ground->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    ground->SetScale(glm::vec3(10.0f));

    Object *girl = engine.AddObject(&girlModel);
    girl->SetPosition(glm::vec3(2.5f,  0.0f, 1.0f));
    girl->Rotate(glm::radians(-105.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    girl->SetScale(glm::vec3(2.0f));

    Object *cylinder = engine.AddObject(&cylinderModel);
    cylinder->SetPosition(glm::vec3(-4.0f, 0.0f, 3.0f));
    cylinder->SetMaterial(&materialSingleColor);

    for (int i = 0; i < 20; i++)
    {
	float x = (float)(rand() % 100) / 10.0f - 5.0f;
	float z = (float)(rand() % 100) / 10.0f - 5.0f;
	glm::vec3 pos = glm::vec3(x * 1.5, 0.0f, z * 1.5);

	Object* grass = engine.AddObject(&grassModel);
	grass->SetPosition(pos);
	grass->Rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	grass->SetMaterial(&materialGrass);
    }

    Light dirLight(glm::vec3(-0.2f, -0.5f, -1.0f), glm::vec3(0.2f), glm::vec3(0.7f), glm::vec3(0.5f));
    engine.AddLight(dirLight);
    Light pointLight(glm::vec3(0.0f, 3.0f, 3.0f), glm::vec3(0.2f), glm::vec3(1.0f, 0.8f, 0.0f), glm::vec3(0.5f), 1.0f, 0.09f, 0.032f);
    engine.AddLight(pointLight);
    engine.SetDefaultMaterial(&materialDefault);
    engine.SetActiveCamera(&camera);
    engine.SetDebugShader(shaderProgramSingleColor);
    

    glm::vec3 pos(0.0f);
    glm::vec3 rot(0);
    glm::vec3 sc(0);
    
    size_t objSelectedIndex = 0;
    Object* objSelected = engine.GetObject(0);
    int lightSelected = 0;
    
    int counter=0;

    while(!glfwWindowShouldClose(engine.GetWindow()))
    // while(!glfwWindowShouldClose(engine.GetWindow()) && counter < 10)
    {
	counter++;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetWindowSize({200, 600});

	ImGui::Begin("Menu", &guiActive);
	
	if (ImGui::CollapsingHeader("Camera")) {
	    ImGui::DragFloat3("Position##1", glm::value_ptr(camera.position), 0.1f);
	    ImGui::DragFloat("FOV", &camera.fov, 0.5f, 1.0f);
	    ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Voxel Models")) {

	    std::vector<std::string> vmodels = {"vmodel.txt", "vcube.txt", "vstair.txt", "vwalls.txt"};

	    if (ImGui::BeginListBox("Model List")) {
		for (size_t i = 0; i < vmodels.size(); i++)
		{
		    if (ImGui::Selectable(vmodels[i].c_str()))
		    {
			voxelmodel.LoadModel(vmodels[i].c_str());
		    }
		}
		ImGui::EndListBox();
	    }
	}

	if (ImGui::CollapsingHeader("Objects")) {

	    // Object list
	    ImGui::Text("%zu objects", engine.GetObjectCount());

	    if (ImGui::BeginListBox("Object list")) {
		for (size_t i = 0; i < engine.GetObjectCount(); i++)
		{
		    std::string name = std::to_string(i) + ".  "
			+ engine.GetObject(i)->GetModel()->GetDirectory();
		    if (ImGui::Selectable(name.c_str(), (objSelectedIndex == i) ? true : false))
		    {
			objSelectedIndex = i;
		    }
		}
		ImGui::EndListBox();
	    }

	    objSelected = engine.GetObject(objSelectedIndex);
	    ImGui::Text("%zu. %s", objSelectedIndex, objSelected->GetModel()->GetDirectory());


	    ImGui::DragFloat3("Position##2", glm::value_ptr(objSelected->position), 0.1f);

	    Drag3Rotation("Rotation##2", objSelected);

	    if (ImGui::Button("Add new object"))
	    {
		Object *obj = engine.AddObject(&catModel);
		obj->SetPosition(pos);
	    }
	}

	if (ImGui::CollapsingHeader("Lights")) {
	    ImGui::Text("%zu ligths", engine.GetLightCount());

	    if (ImGui::BeginListBox("Light list")) {
		for (size_t i = 0; i < engine.GetLightCount(); i++)
		{
		    std::string name = "Light " + std::to_string(i);
		    if (ImGui::Selectable(name.c_str()))
		    {
			lightSelected = i;
		    }
		}
		ImGui::EndListBox();
	    }

	    Light *light = engine.GetLight(lightSelected);

	    ImGui::DragFloat3("Position##3", glm::value_ptr(light->position), 0.1f);
	    ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient),ImGuiColorEditFlags_Float);
	    ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse),ImGuiColorEditFlags_Float);
	    ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular),ImGuiColorEditFlags_Float);
	    ImGui::DragFloat("Type", &light->type, 0.01f);
	    ImGui::DragFloat("Constant", &light->constant, 0.01f);
	    ImGui::DragFloat("Linear", &light->linear, 0.01f);
	    ImGui::DragFloat("Quadratic", &light->quadratic, 0.01f);

	    if (ImGui::Button("Add new light"))
	    {
		Light l(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f);
		engine.AddLight(l);
	    }
	}

	if (ImGui::CollapsingHeader("Display Options")) {
	    ImGui::Text("Debug Mode: %d", engine.debugDrawMode);
	    if (ImGui::RadioButton("None", engine.debugDrawMode == 1)) engine.debugDrawMode = 1;
	    if (ImGui::RadioButton("Normals", engine.debugDrawMode == 2)) engine.debugDrawMode = 2;
	    if (ImGui::RadioButton("UVs", engine.debugDrawMode == 3)) engine.debugDrawMode = 3;
	    ImGui::Checkbox("Draw wireframes", &engine.debugDrawWireframes);
	}

	
	ImGui::End();



	

	processInput(engine.GetWindow());

	glClearColor(0.4f, 0.85f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	engine.Draw();

	// SetMat4(shaderProgramGrass, "view", glm::value_ptr(view));
	// SetMat4(shaderProgramGrass, "projection", glm::value_ptr(projection));
	
	// Rendering
	// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// (Your code calls glfwSwapBuffers() etc.)

	glfwSwapBuffers(engine.GetWindow());

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;  

	glfwPollEvents();
    }
	    
    // clean up

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgramGrass);
    glfwTerminate();
    printf("Bye\n");
    
    return 0;
}
