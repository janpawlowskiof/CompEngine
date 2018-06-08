#include "stdafx.h"
#include "transformComponent.h"

glm::mat4 TransformComponent::GetModelMatrix()	//Get matrix that we use in shader later on
{
	glm::mat4 trans;
	trans = glm::translate(trans, position);
	trans *= glm::toMat4(rotation);
	trans = glm::scale(trans, scale);
	return trans;
}

void TransformComponent::Rotate(glm::quat quaterion)
{
	rotation *= quaterion;
	rotation = glm::normalize(rotation);
}

TransformComponent::~TransformComponent()
{
}
