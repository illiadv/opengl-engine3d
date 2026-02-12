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


class GfxEngine {

public:
    GfxEngine(int screenWidth, int screenHeight);
    ~GfxEngine();
    void SetDefaultMaterial(Material* material);
    void SetActiveCamera(Camera *camera);
    void AddLight(Light);
    void Draw();
    GLFWwindow *GetWindow();
    // template<class... Args> Object *AddObject(Args&&... args);
    Object *AddObject(Model* model);
    void RemoveObject(Object *object);

    size_t GetObjectCount();
    Object* GetObject(size_t index);

    size_t GetLightCount();
    Light* GetLight(size_t index);
    // unsigned int AddShader(const char* vertex, const char* fragment);

private:
    GLFWwindow *window;
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    static constexpr int maxLights = 100;
    static constexpr size_t uboLightsSize = maxLights * 4*sizeof(glm::vec4);

    Material* defaultMaterial;
    Camera *activeCamera = nullptr;

    unsigned int uboMatricies;
    unsigned int uboLights;
};
