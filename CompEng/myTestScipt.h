#pragma once

#include "userScriptComponent.h"
#include "transformComponent.h"
#include <iostream>

class MyTestScript : public UserScriptComponent
{
public: 
	float value = 0;

	void Initialize()
	{
	}

	void Update()
	{
		value += 0.03f;

		BaseObject* light = BaseObject::Find("Car");
		TransformComponent* transform = (TransformComponent*)light->GetComponent("Transform");
		transform->position = glm::vec3(0, sin(value), 0);
	}
};