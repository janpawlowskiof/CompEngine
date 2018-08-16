#pragma once

#include "userScriptComponent.h"
#include "transformComponent.h"
#include <iostream>
#include <vector>
#include "collisionData.h"

class MyTestScript : public UserScriptComponent
{
public:
	float value = 0;

	BaseObject *cubeA, *cubeB, *cubeC, *sphereA;
	MeshComponent* meshA, *meshB, *meshC;

	void Initialize()
	{
		cubeA = BaseObject::Find("Cube1");
		cubeB = BaseObject::Find("Cube2");

		TransformComponent* transformA = cubeA->GetComponent<TransformComponent>();
		TransformComponent* transformB = cubeB->GetComponent<TransformComponent>();


		//transformA->RotateGlobal(glm::angleAxis(0*3.12f / 2.0f, glm::vec3(0, 1, 0)));

		//rigidbodyB->inverseMass = 1.0 / (4.0);
		//rigidbodyB->inverseMass = 1.0 / (4.0);

		//transformA->Rotate(glm::angleAxis(0.12f, glm::vec3(1, 0, 0)));
		//transformA->Rotate(glm::angleAxis(3.14f / 2, glm::vec3(0, 1, 0)));
		//transformA->Rotate(glm::angleAxis(3.14f / 6, glm::vec3(0, 0, 1)));

		//transformB->Rotate(glm::angleAxis(0.7f * interval, glm::vec3(-0.3, 0.1, 0.3)));

		meshA = (MeshComponent*)cubeA->GetComponent<MeshComponent>();
		meshB = (MeshComponent*)cubeB->GetComponent<MeshComponent>();
	}

	void Update()
	{
		float interval = 0.006f;
		value += interval;

		baseObject->GetComponent<TransformComponent>()->position = glm::vec3(2 * glm::sin(value), 1, 0);
	}
};