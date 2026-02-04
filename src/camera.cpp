#include "camera.hpp"


Camera::Camera(
	    glm::vec3 position,
	    glm::vec3 front,
	    glm::vec3 up,
	    float fov,
	    float speed,
	    float sensitivity
	  ) {
	this->position = position;
	this->front = front;
	this->up = up;
	this->fov = fov;
	this->speed = speed;
	this->mouseSensitivity = sensitivity;
	this->pitch = 0;
	this->yaw = -90.0f;
    }

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position,  // pos
			   position + front,  // target
			   up);
}

void Camera::ProcessMovement(CameraDirection direction, float deltaTime)
{
    float velocity = speed * deltaTime;

    if (direction == FORWARD)
	position += velocity * front;
    if (direction == BACKWARD)
	position -= velocity * front;
    if (direction == RIGHT)
	position += velocity * glm::normalize(glm::cross(front, up));
    if (direction == LEFT)
	position -= velocity * glm::normalize(glm::cross(front, up));
    if (direction == UP)
	position += velocity * glm::vec3(0.0f, 1.0f, 0.0f);
    if (direction == DOWN)
	position -= velocity * glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::ProcessLookAround(float xOffset, float yOffset)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
	pitch = 89.0f;
    if (pitch < -89.0f)
	pitch = -89.0f;

    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);

}


void Camera::ProcessZoom(double amount)
{
    fov -= amount * 5.0f;

    if (fov < 1.0f)
	fov = 1.0f;
    if (fov > 90.0f)
	fov = 90.0f;
}
