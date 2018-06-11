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
		cubeC = BaseObject::Find("Cube3");

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		transformA->position = glm::vec3(-0.55f, 0.92f, 0);
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		//transformB->position = glm::vec3(1.0f, 0.0f, 0.0f);
		TransformComponent* transformC = (TransformComponent*)cubeC->GetComponent("Transform");
		transformC->position = glm::vec3(0.0f, 4.0f, 0.0f);

		RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)cubeA->GetComponent("Rigidbody");
		//rigidbodyA->velocity = glm::vec3(1.0, 1.2, 0);

		RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)cubeB->GetComponent("Rigidbody");
		//rigidbodyB->velocity = glm::vec3(-2.0, 0, 0);
		//rigidbodyB->angularVelocity = glm::vec3(0, 0, 2);
		//rigidbodyB->mass = 10;
		rigidbodyB->mass = INFINITY;

		//rigidbodyA->angularVelocity = glm::vec3(0, 1, 4);
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
		TransformComponent* transformC = (TransformComponent*)cubeC->GetComponent("Transform");

		RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)cubeA->GetComponent("Rigidbody");
		RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)cubeB->GetComponent("Rigidbody");

		rigidbodyA->force += glm::vec3(0, -9.81 * rigidbodyA->mass, 0);

		//transformA->position += 0.1f * glm::vec3(glfwGetKey(rendererMaster->window, GLFW_KEY_L) - glfwGetKey(rendererMaster->window, GLFW_KEY_J),
		//	glfwGetKey(rendererMaster->window, GLFW_KEY_U) - glfwGetKey(rendererMaster->window, GLFW_KEY_O),
		//	glfwGetKey(rendererMaster->window, GLFW_KEY_K) - glfwGetKey(rendererMaster->window, GLFW_KEY_I));
		//transformA->Rotate(glm::angleAxis(1.4f * interval, glm::vec3(0, 0, 1)));
		//transformB->Rotate(glm::angleAxis(0.7f * interval, glm::vec3(-0.3, 0.1, 0.3)));

		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		/*CollsionData collisionData;
		if (BoxesCollideGJK(collisionData))
		{
			meshA->diffuseColor = glm::vec3(1, 0, 0);
			transformSphere->position = collisionData.contactPoint;
		}
		else
		{
			meshA->diffuseColor = glm::vec3(0, 1, 0);
			transformSphere->position = glm::vec3(20, 0, 0);
		}
		transformC->rotation = glm::lookAt(glm::vec3(0), collisionData.normal, glm::vec3(0, 1, 0));*/
	}
};