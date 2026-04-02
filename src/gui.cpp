#include "gui.hpp"
#include "voxelmodel.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void Gui::ImguiInit(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void Gui::Drag3Rotation(const char* label, Object* object)
{
    if (ImGui::DragFloat3(label, glm::value_ptr(object->eulerRotation), 1.0f))
    {
	glm::quat q = glm::quat(glm::radians(object->eulerRotation));
	object->SetRotation(q);
    }
}


//public:

Gui::Gui(GfxEngine *engine)
    : engine(engine)
{
    ImguiInit(engine->GetWindow());
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Gui::WantKeyboard() {
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool  Gui::WantMouse() {
    return ImGui::GetIO().WantCaptureMouse;
}

void Gui::DrawVoxelModelsMenu()
{
    if (ImGui::CollapsingHeader("Voxel Models")) {

	std::vector<std::string> vmodels = {"vmodel.txt", "vcube.txt", "vstair.txt", "vwalls.txt"};

	if (ImGui::BeginListBox("Model List")) {
	    for (size_t i = 0; i < vmodels.size(); i++)
	    {
		if (ImGui::Selectable(vmodels[i].c_str()))
		{
		    VoxelModel *voxelmodel = dynamic_cast<VoxelModel*>(engine->GetObject(0)->GetModel());
		    if (voxelmodel)
			voxelmodel->LoadModel(vmodels[i].c_str());
		}
	    }
	    ImGui::EndListBox();
	}
    }
}

void Gui::DrawCameraMenu()
{
    if (ImGui::CollapsingHeader("Camera")) {
	ImGui::DragFloat3("Position##1", glm::value_ptr(engine->GetCamera()->position), 0.1f);
	ImGui::DragFloat("FOV", &engine->GetCamera()->fov, 0.5f, 1.0f);
	ImGui::Separator();
    }
}

void Gui::DrawObjectsMenu()
{
    if (ImGui::CollapsingHeader("Objects")) {

	// Object list
	ImGui::Text("%zu objects", engine->GetObjectCount());

	if (ImGui::BeginListBox("Object list")) {
	    for (size_t i = 0; i < engine->GetObjectCount(); i++)
	    {
		std::string name = std::to_string(i) + ".  "
		    + engine->GetObject(i)->GetModel()->GetDirectory();
		if (ImGui::Selectable(name.c_str(), (m_objSelectedIndex == i) ? true : false))
		{
		    m_objSelectedIndex = i;
		}
	    }
	    ImGui::EndListBox();
	}

	Object* objSelected = engine->GetObject(m_objSelectedIndex);
	ImGui::Text("%zu. %s", m_objSelectedIndex, objSelected->GetModel()->GetDirectory());


	ImGui::DragFloat3("Position##2", glm::value_ptr(objSelected->position), 0.1f);

	Drag3Rotation("Rotation##2", objSelected);

	if (ImGui::Button("Add new object"))
	{
	    Object *obj = engine->AddObject(objSelected->GetModel());
	    obj->SetPosition(objSelected->position);
	}
    }
}

void Gui::DrawLightsMenu()
{
    if (ImGui::CollapsingHeader("Lights")) {
	ImGui::Text("%zu ligths", engine->GetLightCount());

	if (ImGui::BeginListBox("Light list")) {
	    for (size_t i = 0; i < engine->GetLightCount(); i++)
	    {
		std::string lightType = engine->GetLight(i)->type == 1.0f ? "Point" : "Directional";
		std::string name = "Light " + std::to_string(i) + " (" + lightType + ")";
		if (ImGui::Selectable(name.c_str()))
		{
		    m_lightSelectedIndex = i;
		}
	    }
	    ImGui::EndListBox();
	}

	Light *light = engine->GetLight(m_lightSelectedIndex);

	ImGui::DragFloat3("Position##3", glm::value_ptr(light->position), 0.1f);
	ImGui::ColorEdit3("Ambient", glm::value_ptr(light->ambient),ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(light->diffuse),ImGuiColorEditFlags_Float);
	ImGui::ColorEdit3("Specular", glm::value_ptr(light->specular),ImGuiColorEditFlags_Float);
	ImGui::DragFloat("Type", &light->type, 0.01f);
	ImGui::DragFloat("Constant", &light->constant, 0.01f);
	ImGui::DragFloat("Linear", &light->linear, 0.01f);
	ImGui::DragFloat("Quadratic", &light->quadratic, .01f);

	if (ImGui::Button("Add new light"))
	{
	    Light l(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.2f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f);
	    engine->AddLight(l);
	}
    }
}

void Gui::DrawDisplayOptionsMenu()
{
    if (ImGui::CollapsingHeader("Display Options")) {
	ImGui::Text("Debug Mode: %d", engine->debugDrawMode);
	if (ImGui::RadioButton("None", engine->debugDrawMode == 1)) engine->debugDrawMode = 1;
	if (ImGui::RadioButton("Normals", engine->debugDrawMode == 2)) engine->debugDrawMode = 2;
	if (ImGui::RadioButton("UVs", engine->debugDrawMode == 3)) engine->debugDrawMode = 3;
	ImGui::Checkbox("Draw wireframes", &engine->debugDrawWireframes);
	ImGui::Checkbox("Draw light handles", &engine->debugDrawLightHandles);
	ImGui::Checkbox("Draw object handles", &engine->debugDrawObjectHandles);
    }
}

void Gui::Draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetWindowSize({200, 600});

    ImGui::Begin("Menu", &guiActive);


    DrawCameraMenu();
    DrawVoxelModelsMenu();
    DrawObjectsMenu();
    DrawLightsMenu();
    DrawDisplayOptionsMenu();


    ImGui::End();

    // Rendering
    // (Your code clears your framebuffer, renders your other stuff etc.)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // (Your code calls glfwSwapBuffers() etc.)
}
