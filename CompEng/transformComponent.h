#pragma once

#include "component.h"
#include "baseObject.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TransformComponent : public Component
{
private:

public:
	glm::vec3 position = glm::vec3(0, 0,0);	//vec3 position of object
	glm::mat4 GetModelMatrix();	//method to get matrix that we use later on in shader
	float scale = 1.0f;	//scalar

	std::string type() { return "Transform"; }
	TransformComponent(glm::vec3 pos) { position = pos; };
	~TransformComponent();
};

