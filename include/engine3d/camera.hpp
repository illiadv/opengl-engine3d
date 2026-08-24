#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace e3d
{

enum class CameraDirection {
    Forward, Backward, Left, Right, Up, Down
};

enum class CameraProjection {
    Perspective, Othographic
};

class Camera
{
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float pitch = 0;
    float yaw = -90;

    float speed = 4.0f;
    float mouseSensitivity = 0.05f;

    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    CameraProjection projection = CameraProjection::Perspective;
    float fov = 60.0f;
    glm::vec4 orthoSize = {-1.0f, 1.0f, -1.0f, 1.0f};

    Camera();

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(int screenWidth, int screenHeight) const;

    void ProcessMovement(CameraDirection direction, float deltaTime);

    void ProcessLookAround(float xOffset, float yOffset);

    void ProcessZoom(double amount);

};

}

#endif
