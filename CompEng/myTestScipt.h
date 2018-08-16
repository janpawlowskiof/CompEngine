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

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");

		RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)cubeA->GetComponent("Rigidbody");
		RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)cubeB->GetComponent("Rigidbody");
		ColliderComponent* colliderA = (ColliderComponent*)cubeA->GetComponent("Collider");
		ColliderComponent* colliderB = (ColliderComponent*)cubeB->GetComponent("Collider");
		
		rigidbodyA->mass = 1;
		colliderA->RecalculateInteria();
//		rigidbodyB->mass = 100;
		colliderB->RecalculateInteria();

		//rigidbodyA->velocity = glm::vec3(2.0, 0.0, 0);
		rigidbodyA->angularVelocity = glm::vec3(0, 0, -6);

		//transformA->RotateGlobal(glm::angleAxis(0*3.12f / 2.0f, glm::vec3(0, 1, 0)));

		//rigidbodyB->inverseMass = 1.0 / (4.0);
		//rigidbodyB->inverseMass = 1.0 / (4.0);

		//transformA->Rotate(glm::angleAxis(0.12f, glm::vec3(1, 0, 0)));
		//transformA->Rotate(glm::angleAxis(3.14f / 2, glm::vec3(0, 1, 0)));
		//transformA->Rotate(glm::angleAxis(3.14f / 6, glm::vec3(0, 0, 1)));

		//transformB->Rotate(glm::angleAxis(0.7f * interval, glm::vec3(-0.3, 0.1, 0.3)));

		meshA = (MeshComponent*)cubeA->GetComponent("Mesh");
		meshB = (MeshComponent*)cubeB->GetComponent("Mesh");
	}

	void Update()
	{
		float interval = 0.006f;
		value += interval;

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");

		RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)cubeA->GetComponent("Rigidbody");
		RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)cubeB->GetComponent("Rigidbody");

		//rigidbodyA->torque += glm::vec3(0, 0, 20.5);
		rigidbodyA->force += glm::vec3(0, -9.81, 0) * rigidbodyA->GetMass();
		//rigidbodyA->force += glm::vec3(12, 0, 0);

		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();
	}
};