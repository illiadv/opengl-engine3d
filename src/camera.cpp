#include "engine3d/camera.hpp"

Camera::Camera(){}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(position,
			   position + front,  // target
			   up);
}

glm::mat4 Camera::GetProjectionMatrix(int screenWidth, int screenHeight) const
{
    float aspectRatio = (float)screenWidth / (float)screenHeight;
    if (projection == CameraProjection::Perspective)
    {
	return glm::perspective(
	    glm::radians(fov),
	    aspectRatio, nearPlane, farPlane);
    }
    else
    {
	return glm::ortho(orthoSize.x, orthoSize.y, orthoSize.z, orthoSize.w,
		nearPlane, farPlane);
    }
}

void Camera::ProcessMovement(CameraDirection direction, float deltaTime)
{
    float velocity = speed * deltaTime;

    if (direction == CameraDirection::Forward)
	position += velocity * front;
    if (direction == CameraDirection::Backward)
	position -= velocity * front;
    if (direction == CameraDirection::Right)
	position += velocity * glm::normalize(glm::cross(front, up));
    if (direction == CameraDirection::Left)
	position -= velocity * glm::normalize(glm::cross(front, up));
    if (direction == CameraDirection::Up)
	position += velocity * glm::vec3(0.0f, 1.0f, 0.0f);
    if (direction == CameraDirection::Down)
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
