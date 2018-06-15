#pragma once

#include <iostream>
#include <vector>
#include <glm\ext.hpp>
#include <glm\gtx\string_cast.hpp>

struct SupportPoint
{
	glm::vec3 point;	//point on minkowski diffrence
	glm::vec3 supportA;	//point of SuppportA - used to get collision point
	glm::vec3 supportB;

	SupportPoint(glm::vec3 sA, glm::vec3 sB)
	{
		point = sA - sB;
		supportA = sA;
		supportB = sB;
	}
	SupportPoint() {};
};

class Simplex
{
public:
	int count = 0;
	SupportPoint supports[4];
	glm::vec3 normal;	//normal always pointing away from the origin, regardless of the winding

	void Add(SupportPoint A)
	{
		supports[count] = A;
		count++;

		if (count == 3)
			CalculateNormal();
	}

	Simplex(SupportPoint A)
	{
		supports[0] = A;
		count = 1;
	}

	Simplex(SupportPoint A, SupportPoint B)
	{
		supports[0] = B;
		supports[1] = A;
		count = 2;
	}

	Simplex(SupportPoint A, SupportPoint B, SupportPoint C)
	{
		supports[0] = C;
		supports[1] = B;
		supports[2] = A;
		count = 3;
		CalculateNormal();
	}

	Simplex() {};

private:
	void CalculateNormal()
	{
		normal = glm::cross(supports[1].point - supports[0].point, supports[2].point - supports[0].point);	//normal
		if (glm::dot(normal, supports[1].point) < 0)	//normal pointing inwards (we want it outwards)
			normal = -normal;	//we now have normal
		if (normal.x != normal.x)	//NaN error, objects probably touching
			normal = glm::vec3(1, 1, 1);

		normal = glm::normalize(normal);
	}

	//das broken :c
	/*void DebugDraw(glm::vec3 color)
	{
		//glColor3f(0.2f, 0.2f, 0.8f);
		BaseObject* cube = BaseObject::Find("Triangle");
		TransformComponent* transform = (TransformComponent*)cube->GetComponent("Transform");
		glm::mat4 model = transform->GetModelMatrix();
		MeshComponent* mesh = (MeshComponent*)cube->GetComponent("Mesh");
		mesh->diffuseColor = color;
		mesh->specularColor = color;
		mesh->vertices[0].position = supports[0].point;
		mesh->vertices[1].position = supports[1].point;
		mesh->vertices[2].position = supports[2].point;
		glm::vec3 n = glm::normalize(glm::cross(supports[2].point - supports[0].point, supports[1].point - supports[0].point));
		mesh->vertices[0].normal = supports[0].point;
		mesh->vertices[1].normal = supports[1].point;
		mesh->vertices[2].normal = supports[2].point;

		mesh->Initialize();

		rendererMaster->currentShader->setMat4("modelMatrix", model);
		mesh->Draw(rendererMaster->currentShader);
	}*/
};

class ColliderComponent;

class CollisionData
{
public:
	glm::vec3 globalPositionA;
	glm::vec3 globalPositionB;
	glm::vec3 normal;
	float penetration;

	glm::vec3 localPositionA;
	glm::vec3 localPositionB;

	glm::vec3 tangentA;
	glm::vec3 tangentB;

	ColliderComponent* colliderA;
	ColliderComponent* colliderB;

	CollisionData() {};
	CollisionData(glm::vec3 globalPositionA, glm::vec3 globalPositionB, glm::vec3 localPositionA, glm::vec3 localPositionB, glm::vec3 normal, float penetration, ColliderComponent* colliderA, ColliderComponent* colliderB, glm::vec3 tangentA, glm::vec3 tangentB)
	{
		this->globalPositionA = globalPositionA;
		this->globalPositionB = globalPositionB;
		this->localPositionA = localPositionA;
		this->localPositionB = localPositionB;
		this->normal = normal;
		this->penetration = penetration;
		this->colliderA = colliderA;
		this->colliderB = colliderB;
		this->tangentA = tangentA;
		this->tangentB = tangentB;
	}
};