#pragma once

#include "component.h"
#include "baseObject.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

class TransformComponent : public Component
{
private:

public:
	glm::vec3 position = glm::vec3(0, 0,0);	//vec3 position of object
	glm::vec3 scale = glm::vec3(1);
	glm::quat rotation;

	glm::mat4 GetModelMatrix();	//method to get matrix that we use later on in shader
	glm::mat3 GetRotationMatrix();
	void Rotate(glm::quat quaterion);
	void RotateGlobal(glm::quat quaterion);

	//float angle = 0.0f;
	std::string type() { return "Transform"; }
	TransformComponent(glm::vec3 pos) { position = pos; };
	~TransformComponent();
};

