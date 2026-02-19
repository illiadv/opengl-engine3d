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
    void SetDebugShader(unsigned int shader);
    void SetActiveCamera(Camera *camera);
    void SetHandleModel(Model* model);
    void AddLight(Light);
    void Draw();
    GLFWwindow *GetWindow();
    // template<class... Args> Object *AddObject(Args&&... args);
    Object *AddObject(BaseModel* model);
    void RemoveObject(Object *object);

    size_t GetObjectCount();
    Object* GetObject(size_t index);

    size_t GetLightCount();
    Light* GetLight(size_t index);
    // unsigned int AddShader(const char* vertex, const char* fragment);

    bool debugDrawWireframes = false;
    bool debugDrawLightHandles = true;
    bool debugDrawObjectHandles = false;
    int debugDrawMode = 1;

private:
    void DrawHandle(Model* model, glm::vec3 position, glm::vec3 scale, glm::vec3 direciton = glm::vec3({0.0f, 0.0f, 1.0f}));

    GLFWwindow *window;
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    static constexpr int maxLights = 100;
    static constexpr size_t uboLightsSize = maxLights * 4*sizeof(glm::vec4);

    Material* defaultMaterial = nullptr;
    Camera *activeCamera = nullptr;

    unsigned int wireframeShader;
    unsigned int normalShader;
    unsigned int uvShader;

    Model* handleModel = nullptr;

    unsigned int uboMatricies;
    unsigned int uboLights;
};
