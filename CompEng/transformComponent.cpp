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

glm::mat3 TransformComponent::GetRotationMatrix()
{
	return glm::toMat3(rotation);
}

void TransformComponent::Rotate(glm::quat quaterion)
{
	rotation *= quaterion;
	rotation = glm::normalize(rotation);
}

void TransformComponent::RotateGlobal(glm::quat quaterion)
{
	rotation = quaterion * rotation;
	rotation = glm::normalize(rotation);
}

TransformComponent::~TransformComponent()
{
}
