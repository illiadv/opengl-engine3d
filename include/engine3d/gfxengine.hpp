#ifndef GFXENGINE_HPP
#define GFXENGINE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Renderable;
struct Light;
struct Mesh;
struct Camera;
struct Material;


class GfxEngine {

public:
    GfxEngine(int screenWidth, int screenHeight);
    ~GfxEngine();
    void ResizeViewport(int width, int height);
    void BeginFrame(const Camera &camera);
    void EndFrame(const Camera &camera);

    void SubmitMesh(const Mesh *mesh, const Material *material, const glm::mat4 &transform);
    void SubmitModel(const std::vector<Mesh> *model, const Material *material, const glm::mat4 &transform);
    void SubmitLight(const Light *light);

    glm::vec3 backgroundColor = glm::vec3(0.4f, 0.85f, 0.9f);

protected:
    
private:
    std::vector<const Light*> m_lights;
    std::vector<Renderable> m_renderables;
    static constexpr int m_maxLights = 100;
    static constexpr size_t m_uboLightsSize = m_maxLights * 4*sizeof(glm::vec4);

    unsigned int m_uboMatricies;
    unsigned int m_uboLights;

    static GfxEngine *s_instance;

    int m_screenWidth{};
    int m_screenHeight{};
};

#endif
