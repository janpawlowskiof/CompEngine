#include "stdafx.h"
#include "transformComponent.h"

glm::mat4 TransformComponent::GetModelMatrix()	//Get matrix that we use in shader later on
{
	glm::mat4 trans;
	trans = glm::translate(trans, position);
	trans = glm::rotate(trans, angle, glm::vec3(0.2, 0.3, 0.5));
	trans = glm::scale(trans, glm::vec3(scale, scale, scale));
	return trans;
}

TransformComponent::~TransformComponent()
{
}
