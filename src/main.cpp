#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gfxengine.hpp"

#include "voxelmodel.hpp"


int main()
{
    GfxEngine engine(1000, 600);


    unsigned int shaderProgram = CreateShader("vertex.glsl", "fragment.glsl");
    unsigned int shaderProgramGrass = CreateShader("vertex.glsl", "grass.glsl");

    unsigned int shaderProgramSingleColor = CreateShader("vertex.glsl", "singleColor.glsl");

    Material materialDefault = Material(shaderProgram, 64);
    Material materialGrass = Material(shaderProgramGrass, 0);
    // Material materialSingleColor = Material(shaderProgramSingleColor, 0);

    Model backpackModel("assets/backpack/backpack.obj", true);
    Model catModel("assets/cat/cat.obj");
    Model crateModel("assets/crate/crate.obj");
    Model groundModel("assets/ground/ground.obj");
    Model girlModel("assets/pickme/pickme.obj");
    Model grassModel("assets/grass/grass.obj");
    Model cylinderModel("assets/cylinder/cylinder.obj");
    Model lightHandleModel("assets/light-handle/light-handle.obj");

    VoxelModel voxelmodel("vmodel.txt");

    engine.SetHandleModel(&lightHandleModel);

    // Object* backpack = engine.AddObject(&backpackModel);
    //
    // backpack->SetPosition(glm::vec3(2.0, 1.0, 1.0f));
    // backpack->SetScale(glm::vec3(0.3f));


    Object *voxels = engine.AddObject(&voxelmodel);
    voxels->SetPosition(glm::vec3(-10.0f, 0.0f, -10.0f));

    Object *crate = engine.AddObject(&crateModel);
    // crate->SetScale(glm::vec3(0.1f, 0.1f, 1.0f));
    crate->SetPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    Object *cat = engine.AddObject(&catModel);
    (void)cat;
    // cat->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
    //
    Object *test = engine.AddObject(&lightHandleModel);
    test->SetPosition({2, 0, 6});
    
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
    // cylinder->SetMaterial(&materialSingleColor);

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
    Camera camera;
    engine.SetCamera(&camera);
    engine.SetDebugShader(shaderProgramSingleColor);
    
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while(!glfwWindowShouldClose(engine.GetWindow()))
    {
	engine.processInput(deltaTime);


	engine.Draw();

	// SetMat4(shaderProgramGrass, "view", glm::value_ptr(view));
	// SetMat4(shaderProgramGrass, "projection", glm::value_ptr(projection));
	

	glfwSwapBuffers(engine.GetWindow());

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;  

	glfwPollEvents();
    }
	    
    // clean up

    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgramGrass);
    glfwTerminate();
    printf("Bye\n");
    
    return 0;
}
