#ifndef GUI_HPP
#define GUI_HPP

#include "gfxengine.hpp"

class Gui {
private:
    void ImguiInit(GLFWwindow* window);
    void Drag3Rotation(const char* label, Object* object);

    bool guiActive = false;
    GfxEngine *engine;
    int drawMode = 1;

    size_t m_objSelectedIndex = 0;
    size_t m_lightSelectedIndex = 0;
public:
    ~Gui();
    Gui(GfxEngine *engine);
    bool WantKeyboard();
    bool WantMouse();
    void DrawVoxelModelsMenu();
    void DrawCameraMenu();
    void DrawDisplayWorldMenu();
    void DrawObjectsMenu();
    void DrawLightsMenu();
    void DrawDisplayOptionsMenu();
    void Draw();
};

#endif
