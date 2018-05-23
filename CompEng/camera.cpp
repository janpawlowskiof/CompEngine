#include "stdafx.h"
#include "camera.h"

void Camera::Update(GLFWwindow* window)
{
	float vertical = ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) ? 1 : 0) + ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) ? -1 : 0);
	float horizontal = ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ? 1 : 0) + ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ? -1 : 0);
	if (horizontal != 0 || vertical != 0)
	{
		glm::vec3 dir = glm::normalize(glm::vec3(-horizontal, 0, vertical));
		Move(dir * speed);
	}
	
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	Rotate(-(mouseY - oldMouseY) * sensitivity, (mouseX - oldMouseX) * sensitivity);
	oldMouseX = mouseX;
	oldMouseY = mouseY;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + forward, up);
}

void Camera::Move(glm::vec3 offset)
{
	position += forward*offset.z + right*offset.x + up*offset.y;
}

void Camera::Rotate(float pitch, float yaw)
{
	this->pitch += pitch;
	this->yaw += yaw;

	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	RecalculateVectors();
}

void Camera::RecalculateVectors()
{
	forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward.y = sin(glm::radians(pitch));
	forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = glm::normalize(forward);
	right = glm::normalize(glm::cross(glm::vec3(0, 1.0f, 0), forward));
	up = glm::cross(forward, right);
}

void Camera::LookAt(glm::vec3 target)
{
	forward = glm::normalize(target - position);
	right = glm::normalize(glm::cross(glm::vec3(0, 1.0f, 0), forward));
	up = glm::cross(forward, right);
}

Camera::Camera(glm::vec3 position)
{
	this->position = position;
	//forward = glm::vec3(0.0f, 0.0f, -1.0f);
	RecalculateVectors();
}

void Camera::Initialize(GLFWwindow* window)
{
	glfwGetCursorPos(window, &oldMouseX, &oldMouseY);
}


Camera::~Camera()
{
}
