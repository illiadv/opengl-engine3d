#include <algorithm>
#include <stdexcept>

#include "glad/gl.h"
#include "engine3d/gfxengine.hpp"
#include "engine3d/camera.hpp"
#include "engine3d/light.hpp"
#include "engine3d/mesh.hpp"
#include "engine3d/material.hpp"
#include "util.hpp"
#include "stb_image.h"

namespace e3d
{

struct RenderCommand
{
    // To minimize state changes, commands are sorted by shader, then by material and then by mesh.
    uint32_t sortKey;
    const Mesh *mesh;
    const Material *material;
    glm::mat4 transform;

    RenderCommand(const Mesh *mesh, const Material *material, glm::mat4 transform)
	: mesh(mesh), material(material), transform(transform)
    {
	// 255 different shaders, 255 materials and 65536 meshes are supported
	sortKey = material->GetShader()->GetID() << 24 |
	    material->GetID() | 16;
	    mesh->GetID();
    }
};


GfxEngine::GfxEngine(int screenWidth, int screenHeight, void (*(*loadFunc)(const char *))())
{
    // Load OpenGL function definitions
    int version = gladLoadGL(loadFunc);
    if (version == 0)
    {
	throw std::runtime_error("Failed to initialize OpenGL context");
    }
    printf("Loaded OpenGL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
    // The engine depends on OpenGL context version 3.3 (or higher) which is provided by the host
    if (GLAD_VERSION_MAJOR(version) < 3 || (GLAD_VERSION_MAJOR(version) == 3 && GLAD_VERSION_MINOR(version) < 3))
    {
	throw std::runtime_error("Engine Error: The Host Application did not create an OpenGL 3.3+ context!");
    }

    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    glViewport(0, 0, screenWidth, screenHeight);

    stbi_set_flip_vertically_on_load(1);

    // Create a UBO for view and projection matricies. Also viewPos vec3 lives there.
    glCall(glGenBuffers(1, &m_uboMatricies));

    // Allocate space for it
    constexpr unsigned int uboMatriciesSize =
	2 * sizeof(glm::mat4)
	+ sizeof (glm::vec3);
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferData(GL_UNIFORM_BUFFER, uboMatriciesSize, NULL, GL_STATIC_DRAW));

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    // Bind UBO to binding point 0
    glCall(glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatricies, 0, uboMatriciesSize));

    // Create a UBO light data
    glCall(glGenBuffers(1, &m_uboLights));

    constexpr unsigned int uboLigthsSize =
	m_lightsArraySize
	+ sizeof(unsigned int);
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferData(GL_UNIFORM_BUFFER, uboLigthsSize, NULL, GL_STATIC_DRAW));

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    // Bind UBO to binding point 1
    glCall(glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_uboLights, 0, uboLigthsSize));

    // Apply default render state manually
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glEnable(GL_CULL_FACE));
    glCall(glCullFace(GL_BACK));
    glCall(glDisable(GL_BLEND));
    glCall(glDepthMask(GL_TRUE));
}

void GfxEngine::ResizeViewport(int width, int height)
{
    m_screenWidth = width;
    m_screenHeight = height;
    glViewport(0, 0, width, height);
}

GfxEngine::~GfxEngine()
{
}

void GfxEngine::SubmitLight(const Light *light)
{
    if (m_lights.size() < m_maxLights)
	m_lights.push_back(light);
}

void GfxEngine::SubmitMesh(const Mesh *mesh, const Material *material, const glm::mat4 &transform)
{
    RenderCommand command(mesh, material, transform);
    if (command.material->GetRenderState().blending == BlendMode::None)
    {
	m_queueOpaque.push_back(command);
    }
    else
    {
	m_queueTransparent.push_back(command);
    }
}

void GfxEngine::BeginFrame(const Camera &camera, const Framebuffer *framebuffer)
{
    // Remember camera position. Needed to sort transparent queue by distance to the
    // camera in EndFrame.
    m_currentCameraPosition = camera.position;

    unsigned int framebufferID;
    if (framebuffer == nullptr)
    {
	framebufferID = 0;
    }
    else
    {
	framebufferID = framebuffer->GetID();
    }

    // Bind the framebuffer
    if (framebufferID != m_boundFramebufferID)
    {
	glCall(glBindFramebuffer(GL_FRAMEBUFFER, framebufferID));
	m_boundFramebufferID = framebufferID;
    }

    m_queueOpaque.clear();
    m_lights.clear();

    // Clear the framebuffer
    glCall(glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f));
    glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    // Pass data to the Matrices UBO
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix(m_screenWidth, m_screenHeight);

    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatricies));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection)));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 2*sizeof(glm::mat4), sizeof(glm::vec3), glm::value_ptr(camera.position)));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

}

void GfxEngine::LightConverter::ConsumeDirectionalLight(const DirectionalLight* light)
{
    result = LightGPU {
	.position = glm::vec4(light->direction.x, light->direction.y, light->direction.z, 0.0f),
	.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, 0.0f),
	.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, 0.0f),
	.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, 0.0f),
    };
}

void GfxEngine::LightConverter::ConsumePointLight(const PointLight* light)
{
    result = LightGPU {
	.position = glm::vec4(light->position.x, light->position.y, light->position.z, 1.0f),
	.ambient = glm::vec4(light->ambient.x, light->ambient.y, light->ambient.z, light->constant),
	.diffuse = glm::vec4(light->diffuse.x, light->diffuse.y, light->diffuse.z, light->linear),
	.specular = glm::vec4(light->specular.x, light->specular.y, light->specular.z, light->quadratic),
    };
}

void GfxEngine::ApplyRenderState(const RenderState &state)
{
    if (state == m_currentRenderState)
	return;

    // Depth testing
    if (state.depthTesting != m_currentRenderState.depthTesting)
    {
	if (state.depthTesting)
	    glEnable(GL_DEPTH_TEST);
	else
	    glDisable(GL_DEPTH_TEST);
    }

    // Depth writing
    if (state.depthWriting != m_currentRenderState.depthWriting)
    {
	if (state.depthWriting)
	{
	    glCall(glDepthMask(GL_TRUE));
	}
	else
	{
	    glCall(glDepthMask(GL_FALSE));
	}
    }

    // Face culling
    if (state.culling != m_currentRenderState.culling)
    {
	if (state.culling == CullMode::None)
	{
	    glDisable(GL_CULL_FACE);
	}
	else if (state.culling == CullMode::Back)
	{
	    glEnable(GL_CULL_FACE);
	    glCullFace(GL_BACK);
	}
    }

    // Blending
    if (state.blending != m_currentRenderState.blending)
    {
	if (state.blending == BlendMode::None)
	{
	    glDisable(GL_BLEND);
	}
	else if (state.blending == BlendMode::Alpha)
	{
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
    }

    m_currentRenderState = state;
}

void GfxEngine::ExecuteRenderCommand(RenderCommand &command)
{
    glm::mat3 normalMat;
    normalMat = glm::transpose(glm::inverse(/* view * */ command.transform));

    unsigned int shaderID = command.material->GetShader()->GetID();
    unsigned int materialID = command.material->GetID();
    unsigned int meshID = command.mesh->GetID();

    if (shaderID != m_boundShaderID)
    {
	command.material->GetShader()->Bind();
	m_boundShaderID = shaderID;
    }

    if (materialID != m_boundMaterialID)
    {
	command.material->Bind();
	m_boundMaterialID = materialID;
    }

    command.material->GetShader()->SetMat4("model", glm::value_ptr(command.transform));
    command.material->GetShader()->SetMat3("normalMat", glm::value_ptr(normalMat));

    if (meshID != m_boundMeshID)
    {
	command.mesh->Bind();
	m_boundMeshID = meshID;
    }
    command.mesh->Draw();
}

void GfxEngine::EndFrame()
{
    LightGPU ligtsGPU[m_maxLights]{};

    for (size_t i = 0; i < m_lights.size(); i++)
    {
	LightConverter converter;
	const Light *light = m_lights[i];
	light->Accept(converter);
	ligtsGPU[i] = converter.result;
    }

    unsigned int numActiveLigths = m_lights.size();

    // Pass data to the Lights UBO
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_lightsArraySize, ligtsGPU));
    glCall(glBufferSubData(GL_UNIFORM_BUFFER, m_lightsArraySize, sizeof(unsigned int), &numActiveLigths));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    // Sort render queues
    auto queueSorter = [](const RenderCommand &a, const RenderCommand &b)
    {
	return a.sortKey < b.sortKey;
    };

    auto transparentQueueSorter = [&](const RenderCommand &a, const RenderCommand &b)
    {
	float distanceA = glm::length(glm::vec3(a.transform[3]) - m_currentCameraPosition);
	float distanceB = glm::length(glm::vec3(b.transform[3]) - m_currentCameraPosition);
	return distanceA > distanceB;
    };

    std::sort(m_queueOpaque.begin(), m_queueOpaque.end(), queueSorter);
    std::sort(m_queueTransparent.begin(), m_queueTransparent.end(), transparentQueueSorter);


    // Draw
    for (auto command : m_queueOpaque)
    {
	ApplyRenderState(command.material->GetRenderState());
	ExecuteRenderCommand(command);
    }

    for (auto command : m_queueTransparent)
    {
	ApplyRenderState(command.material->GetRenderState());
	ExecuteRenderCommand(command);
    }
}

int GfxEngine::GetScreenWidth() const
{
    return m_screenWidth;
}

int GfxEngine::GetScreenHeight() const
{
    return m_screenHeight;
}

}
