#include "stdafx.h"
#include "transformComponent.h"

glm::mat4 TransformComponent::GetModelMatrix()	//Get matrix that we use in shader later on
{
	glm::mat4 trans;
	trans = glm::translate(trans, position);
	return trans;
}

TransformComponent::~TransformComponent()
{
}
