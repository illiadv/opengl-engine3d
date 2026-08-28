# OpenGL 3D rendering engine

An OpenGL-based 3D rendering engine for my projects. The engine is work in progress, more features will be added as needed.

## Aims of the project:
- Abstract away the complexity of OpenGL.
- Provide maximum control over rendering to the application.
- Make it possible to use other backends in the future (e.g. Vulkan).

## Features:
- Targeting OpenGL 3.3 core profile.
- No dependency on any windowing libraries.
- Support for meshes with arbitraty vertex attributes.
- Flexible material system with multiple texture slots. Fallback textures.
- Support for framebuffer objects.
- Dynamic lighting system. Light data is passed to shaders via uniform buffers.
- Optional model and material loading via [assimp](https://github.com/assimp/assimp) library.

## Usage example:

```cpp
// Assuming a GLFW window and OpenGL 3.3+ context are already created
e3d::GfxEngine engine(1280, 720, glfwGetProcAddress);

float quadVertices[] = {  
// positions   // texCoords
 -1.0f,  1.0f,  0.0f, 1.0f,
 -1.0f, -1.0f,  0.0f, 0.0f,
  1.0f, -1.0f,  1.0f, 0.0f,
  1.0f,  1.0f,  1.0f, 1.0f
};	

e3d::BufferLayout quadLayout({
    { e3d::AttributeType::Float, 2 }, // positions
    { e3d::AttributeType::Float, 2 }  // texCoords
});

auto shader = std::make_shared<e3d::Shader>("vertex.glsl", "fragment.glsl");

e3d::Mesh mesh(quadVertices, sizeof(quadVertices), {0, 1, 2, 0, 2, 3}, quadLayout);

auto texture = std::make_shared<e3d::Texture2D>("texture.png");
e3d::Material quadMaterial(quadShader);
quadMaterial.SetTexture("material.diffuse", texture);

e3d::Camera camera;
camera.position = glm::vec3(0.0f, 0.0f, -2.0f);

while(!glfwWindowShouldClose(window))
{
	engine.BeginFrame(camera);

	engine.SubmitMesh(&mesh, &material, glm::mat4(1.0f));

	engine.EndFrame();

	glfwSwapBuffers(window);
	glfwPollEvents();
}
```
