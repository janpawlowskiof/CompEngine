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

	bool BoxesCollide(TransformComponent* transformA, TransformComponent* transformB)
	{
		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		glm::vec3 T = transformB->position - transformA->position;

		glm::vec4 X = glm::vec4(1, 0, 0, 1);
		glm::vec4 Y = glm::vec4(0, 1, 0, 1);
		glm::vec4 Z = glm::vec4(0, 0, 1, 1);
		glm::vec4 zero = glm::vec4(0, 0, 0, 1);

		glm::vec4 zeroA = modelA * zero;
		glm::vec3 A[3];
		A[0] = glm::normalize(modelA * X - zeroA);
		A[1] = glm::normalize(modelA * Y - zeroA);
		A[2] = glm::normalize(modelA * Z - zeroA);

		glm::vec4 zeroB = modelB * zero;
		glm::vec3 B[3];
		B[0] = glm::normalize(modelB * X - zeroB);
		B[1] = glm::normalize(modelB * Y - zeroB);
		B[2] = glm::normalize(modelB * Z - zeroB);

		for (int i = 0; i < 3; ++i)
			for (int j = i; j < 3; ++j)
			{
				if (A[i] == B[j])
					continue;
				glm::vec3 L = glm::normalize(glm::cross(A[i], B[j]));
				float rA = abs(glm::dot(A[0], L)) * transformA->scale.x +
					abs(glm::dot(A[1], L)) * transformA->scale.y +
					abs(glm::dot(A[2], L)) * transformA->scale.z;


				float rB = abs(glm::dot(B[0], L)) * transformB->scale.x +
					abs(glm::dot(B[1], L)) * transformB->scale.y +
					abs(glm::dot(B[2], L)) * transformB->scale.z;

				if (glm::length(glm::dot(T, L)) > rA + rB)
					return false;
			}

		for (int i = 0; i < 3; ++i)
		{
			glm::vec3 L = A[i];
			float rA = abs(glm::dot(A[0], L)) * transformA->scale.x +
				abs(glm::dot(A[1], L)) * transformA->scale.y +
				abs(glm::dot(A[2], L)) * transformA->scale.z;


			float rB = abs(glm::dot(B[0], L)) * transformB->scale.x +
				abs(glm::dot(B[1], L)) * transformB->scale.y +
				abs(glm::dot(B[2], L)) * transformB->scale.z;

			if (glm::length(glm::dot(T, L)) > rA + rB)
				return false;
		}

		for (int i = 0; i < 3; ++i)
		{
			glm::vec3 L = B[i];
			float rA = abs(glm::dot(A[0], L)) * transformA->scale.x +
				abs(glm::dot(A[1], L)) * transformA->scale.y +
				abs(glm::dot(A[2], L)) * transformA->scale.z;


			float rB = abs(glm::dot(B[0], L)) * transformB->scale.x +
				abs(glm::dot(B[1], L)) * transformB->scale.y +
				abs(glm::dot(B[2], L)) * transformB->scale.z;

			if (glm::length(glm::dot(T, L)) > rA + rB)
				return false;
		}

		return true;
	}

	void Update()
	{
		value += 0.01f;

		BaseObject* cubeA = BaseObject::Find("Cube1");
		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		MeshComponent* meshA = (MeshComponent*)cubeA->GetComponent("Mesh");
		transformA->position = glm::vec3(1.8*sin(1.5*value) - 2.1f, 1.0, 0);
		transformA->Rotate(glm::angleAxis(0.01f, glm::vec3(0, 1, 0)));

		BaseObject* cubeB = BaseObject::Find("Cube2");
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		MeshComponent* meshB = (MeshComponent*)cubeB->GetComponent("Mesh");
		transformB->position = glm::vec3(0.0f, 1.0f, 0.0f);
		//transformB->angle = value * -0.65;

		if (BoxesCollide(transformA, transformB))
		{
			meshA->diffuseColor = glm::vec3(1, 0, 0);
		}
		else
		{
			meshA->diffuseColor = glm::vec3(0, 1, 0);
		}
	}
};