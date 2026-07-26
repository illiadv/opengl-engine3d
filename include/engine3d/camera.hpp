#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraDirection {
    FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
};

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    float pitch;
    float yaw;

    float fov;

    float speed;
    float mouseSensitivity;

    float nearPlane;
    float farPlane;

    Camera(
	    glm::vec3 position = glm::vec3(0.0f),
	    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
	    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
	    float fov = 60.0f,
	    float speed = 4.0f,
	    float sensitivity = 0.05f,
	    float nearPlane = 0.1f,
	    float farPlane = 100.0f
	  );

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(int screenWidth, int screenHeight) const;

    void ProcessMovement(CameraDirection direction, float deltaTime);

    void ProcessLookAround(float xOffset, float yOffset);

    void ProcessZoom(double amount);

};
#endif
