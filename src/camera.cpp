#include "engine3d/camera.hpp"

namespace e3d
{

Camera::Camera(){}

glm::mat4 Camera::GetViewMatrix() const
{
    glm::mat4 rotation = glm::toMat4(transform.GetRotation());
    glm::vec3 forward = -glm::vec3(rotation[2]);
    glm::vec3 up = glm::vec3(rotation[1]);
    return glm::lookAt(transform.GetTranslation(),
		       transform.GetTranslation() + forward, // target
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
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    glm::mat4 rotation = glm::toMat4(transform.GetRotation());
    glm::vec3 forward = -glm::vec3(rotation[2]);
    glm::vec3 right = glm::vec3(rotation[0]);

    if (direction == CameraDirection::Forward)
	transform.Translate(velocity * forward);
    if (direction == CameraDirection::Backward)
	transform.Translate(-velocity * forward);
    if (direction == CameraDirection::Right)
	transform.Translate(velocity * right);
    if (direction == CameraDirection::Left)
	transform.Translate(-velocity * right);
    if (direction == CameraDirection::Up)
	transform.Translate(velocity * worldUp);
    if (direction == CameraDirection::Down)
	transform.Translate(velocity * -worldUp);
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

    glm::vec3 eulerAngles(glm::radians(pitch), glm::radians(-yaw), 0.0f);
    transform.SetRotation(eulerAngles);

}

void Camera::ProcessZoom(double amount)
{
    fov -= amount * 5.0f;

    if (fov < 1.0f)
	fov = 1.0f;
    if (fov > 90.0f)
	fov = 90.0f;
}

}
