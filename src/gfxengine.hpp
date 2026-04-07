#ifndef GFXENGINE_HPP
#define GFXENGINE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"

class Gui;

class GfxEngine {

public:
    GfxEngine(int screenWidth, int screenHeight);
    ~GfxEngine();
    void SetDefaultMaterial(Material* material);
    void SetDebugShader(unsigned int shader);
    void SetCamera(Camera *camera);
    void SetHandleModel(Model* model);
    void AddLight(Light);
    void Draw();
    GLFWwindow *GetWindow();
    Camera *GetCamera();
    Object *AddObject(BaseModel* model);
    void RemoveObject(Object *object);

    size_t GetObjectCount();
    Object* GetObject(size_t index);

    size_t GetLightCount();
    Light* GetLight(size_t index);

    void processInput(float deltaTime);

    bool debugDrawWireframes = false;
    bool debugDrawLightHandles = true;
    bool debugDrawObjectHandles = true;
    int debugDrawMode = 1;

protected:
    virtual void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    virtual void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    
private:
    void DrawObjectOverlayed(Model* model, glm::vec3 position, glm::vec3 scale, glm::vec3 direciton, glm::vec4 viewport, glm::mat4 projection);

    static void s_framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void s_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void s_mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void s_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void s_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


    GLFWwindow *m_window;
    float mouseLastX = 400;
    float mouseLastY = 300;


    std::vector<Object*> m_objects;
    std::vector<Light*> m_lights;
    static constexpr int m_maxLights = 100;
    static constexpr size_t m_uboLightsSize = m_maxLights * 4*sizeof(glm::vec4);

    Material* m_defaultMaterial = nullptr;
    Camera *m_camera = nullptr;

    unsigned int m_wireframeShader;
    unsigned int m_normalShader;
    unsigned int m_uvShader;

    Model* m_handleModel = nullptr;

    Gui *gui;

    unsigned int m_uboMatricies;
    unsigned int m_uboLights;

    static GfxEngine *s_instance;
};

#endif
