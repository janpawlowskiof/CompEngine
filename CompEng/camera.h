#pragma once

#include "stdafx.h"
#include <iostream>
#include <glm\glm.hpp>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 forward;
	float pitch = 0;
	float yaw = -90;
	void RecalculateVectors();
public:
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::mat4 GetViewMatrix();
	void Rotate(float pitch, float yaw);
	void Move(glm::vec3);
	void LookAt(glm::vec3);

	void Update(GLFWwindow*);
	void Initialize(GLFWwindow*);

	float speed = 0.05f;
	float sensitivity = 0.2f;
	double oldMouseX = 0, oldMouseY = 0;

	Camera(glm::vec3 position);
	~Camera();
};

