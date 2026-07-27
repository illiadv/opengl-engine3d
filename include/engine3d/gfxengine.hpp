#ifndef GFXENGINE_HPP
#define GFXENGINE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "light.hpp"

struct RenderCommand;
struct Mesh;
struct Camera;
struct Material;


class GfxEngine {

public:
    GfxEngine(int screenWidth, int screenHeight, void (*(*loadFunc)(const char *))());
    ~GfxEngine();
    void ResizeViewport(int width, int height);
    void BeginFrame(const Camera &camera);
    void EndFrame();

    void SubmitMesh(const Mesh *mesh, const Material *material, const glm::mat4 &transform);
    void SubmitLight(const Light *light);

    glm::vec3 backgroundColor = glm::vec3(0.4f, 0.85f, 0.9f);

    int GetScreenWidth() const;
    int GetScreenHeight() const;

private:
    struct LightGPU {
	glm::vec4 position; // w is type (0 = DirLight, 1 = PointLight)
    	glm::vec4 ambient;
    	glm::vec4 diffuse;
    	glm::vec4 specular;
    };

    class LightConverter : public LightConsumer
    {
    public:
	LightGPU result;
	void ConsumeDirectionalLight(const DirectionalLight* light) override;
	void ConsumePointLight(const PointLight* light) override;
    };


    std::vector<const Light*> m_lights;
    std::vector<RenderCommand> m_renderQueue;
    static constexpr int m_maxLights = 100;
    static constexpr unsigned int m_lightsArraySize = m_maxLights * 4*sizeof(glm::vec4);

    unsigned int m_uboMatricies;
    unsigned int m_uboLights;

    static GfxEngine *s_instance;

    int m_screenWidth{};
    int m_screenHeight{};
};

#endif
