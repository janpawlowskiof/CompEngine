#pragma once

#include "userScriptComponent.h"
#include "transformComponent.h"
#include <iostream>

class Simplex
{
public:
	int count = 0;
	glm::vec3 P[4];

	void Add(glm::vec3 A)
	{
		P[count] = A;
		count++;
	}

	Simplex(glm::vec3 A)
	{
		P[0] = A;
		count = 1;
	}

	Simplex(glm::vec3 A, glm::vec3 B)
	{
		P[0] = B;
		P[1] = A;
		count = 2;
	}

	Simplex(glm::vec3 A, glm::vec3 B, glm::vec3 C)
	{
		P[0] = C;
		P[1] = B;
		P[2] = A;
		count = 3;
	}
};

class MyTestScript : public UserScriptComponent
{
public:
	float value = 0;

	BaseObject* cubeA;
	BaseObject* cubeB;
	MeshComponent* meshA;
	MeshComponent* meshB;

	glm::vec4 boxPoints[8] = {
		glm::vec4(-1, 1, 1, 1),
		glm::vec4(1, 1, 1, 1),
		glm::vec4(1, -1, 1, 1),
		glm::vec4(-1, -1, 1, 1),
		glm::vec4(-1, 1, -1, 1),
		glm::vec4(1, 1, -1, 1),
		glm::vec4(1, -1, -1, 1),
		glm::vec4(-1, -1, -1, 1) };

	glm::vec3 pointsA[8];
	glm::vec3 pointsB[8];

	bool BoxesCollideSAT(TransformComponent* transformA, TransformComponent* transformB)
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

		float minPenetration = INFINITY;
		glm::vec3 colisionPoint;

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

				float TL = glm::dot(T, L);
				if (TL > rA + rB)
				{
					return false;
				}
				/*else
				{
					float penetration = rA + rB - TL;
					if (penetration < minPenetration)
					{
						minPenetration = penetration;
					}
				}*/
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

			float TL = glm::dot(T, L);
			if (TL > rA + rB)
			{

				return false;
			}
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

			float TL = glm::dot(T, L);
			if (TL > rA + rB)
			{

				return false;
			}
		}

		return true;
	}

	glm::vec3 SupportA(glm::vec3 D)
	{
		glm::vec3 furthestPoint = pointsA[0];
		float furthestDistance = glm::dot(D, pointsA[0]);
		for (int i = 1; i < 8/*!!!*/; ++i)
		{
			float distance = glm::dot(D, pointsA[i]);
			if (distance > furthestDistance)
			{
				furthestDistance = distance;
				furthestPoint = pointsA[i];
			}
		}
		return furthestPoint;
	}
	glm::vec3 SupportB(glm::vec3 D)
	{
		glm::vec3 furthestPoint = pointsB[0];
		float furthestDistance = glm::dot(D, pointsB[0]);
		for (int i = 1; i < 8/*!!!*/; ++i)
		{
			float distance = glm::dot(D, pointsB[i]);
			if (distance > furthestDistance)
			{
				furthestDistance = distance;
				furthestPoint = pointsB[i];
			}
		}
		return furthestPoint;
	}
	glm::vec3 Support(glm::vec3 D)
	{
		return SupportA(D) - SupportB(-D);
	}

	bool DoSimplex(Simplex& simplex, glm::vec3& D)
	{
		if (simplex.count == 2)
		{
			glm::vec3 a = simplex.P[1];
			glm::vec3 b = simplex.P[0];

			glm::vec3 ab = b - a;
			glm::vec3 ao = -a;

			simplex = Simplex(a, b);	//????
			D = glm::cross(glm::cross(ab, ao), ab);

			return false;
		}
		else if (simplex.count == 3)
		{
			glm::vec3 a = simplex.P[2];
			glm::vec3 b = simplex.P[1];
			glm::vec3 c = simplex.P[0];

			glm::vec3 ao = -a;

			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 abc = glm::cross(ab, ac);	//normal

			//glm::vec3 abPrep = glm::cross(abc, ab);	//Point outwards of the triangle
			//glm::vec3 acPrep = glm::cross(ac, abc);	//Point outwards of the triangle

			if (glm::dot(glm::cross(ab, abc), ao) > 0)
			{
				simplex = Simplex(a, b);
				D = glm::cross(glm::cross(ab, ao), ab);
				return false;
			}

			if (glm::dot(glm::cross(abc, ac), ao) > 0)
			{
				simplex = Simplex(a, c);
				D = glm::cross(glm::cross(ac, ao), ac);
				return false;
			}

			if (glm::dot(abc, ao) > 0)
			{
				D = abc;
				return false;
			}
			D = -abc;
			return false;

			/*if (glm::dot(acPrep, ao) > 0)	//0,0 outside of the tringle
			{
				if (glm::dot(ac, ao) > 0)
				{
					simplex = Simplex(a, c);
					D = glm::cross(glm::cross(ac, ao), ac);
				}
				else
				{
					if (glm::dot(ab, ao) > 0)
					{
						simplex = Simplex(a, b);
						D = glm::cross(glm::cross(ab, ao), ab);
					}
					else
					{
						simplex = Simplex(a);
						D = ao;
					}
				}
			}
			else
			{
				if (glm::dot(abPrep, ao) > 0)
				{
					if (glm::dot(ab, ao) > 0)
					{
						simplex = Simplex(a,b);
						D = glm::cross(glm::cross(ab, ao), ab);
					}
					else
					{
						simplex = Simplex(a);
						D = ao;
					}
				}
				else
				{
					if (glm::dot(abc, ao) > 0)
					{
						simplex = Simplex(a, b, c);
						D = abc;
					}
					else
					{
						simplex = Simplex(a, c, b);
						D = -abc;
					}
				}
			}*/
		}
		else if (simplex.count == 4)
		{
			glm::vec3 a = simplex.P[3];
			glm::vec3 b = simplex.P[2];
			glm::vec3 c = simplex.P[1];
			glm::vec3 d = simplex.P[0];
			glm::vec3 ao = -a;

			glm::vec3 ab = b - a;
			glm::vec3 ac = c - a;
			glm::vec3 ad = d - a;

			if (glm::dot(glm::cross(ab, ac), ao) > 0)
			{
				simplex = Simplex(a, b, c);
			}
			else if (glm::dot(glm::cross(ac, ad), ao) > 0)
			{
				simplex = Simplex(a, c, d);
			}
			else if (glm::dot(glm::cross(ad, ab), ao) > 0)
			{
				simplex = Simplex(a, d, b);
			}
			else
			{
				return true;	//NICEEEEEEEE
			}

			//What to do with our selected face
			a = simplex.P[2];
			b = simplex.P[1];
			c = simplex.P[0];
			ab = b - a;
			ac = c - a;
			glm::vec3 abc = glm::cross(ab, ac);

			if (glm::dot(glm::cross(ab, abc), ao) > 0)
			{
				simplex = Simplex(a, b);
				D = glm::cross(glm::cross(ab, ao), ab);
				return false;
			}
			else if (glm::dot(glm::cross(abc, ac), ao) > 0)
			{
				simplex = Simplex(a, c);
				D = glm::cross(glm::cross(ac, ao), ac);
				return false;
			}

			simplex = Simplex(a, b, c);
			D = abc;
			return false;
		}
	}

	bool BoxesCollideGJK()
	{
		glm::vec3 S = Support(glm::vec3(0, 1, 0));	//random dir
		Simplex simplex(S);
		glm::vec3 D = -S;

		int counter = 0;
		while (counter < 64)
		{
			glm::vec3 A = Support(D);
			if (glm::dot(A, D) < 0)
				return false;
			simplex.Add(A);
			if (DoSimplex(simplex, D))
				return true;
			counter++;
		}
		return false;

	}

	void Initialize()
	{
		cubeA = BaseObject::Find("Cube1");
		cubeB = BaseObject::Find("Cube2");
		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		transformA->Rotate(glm::angleAxis(1.01f, glm::vec3(0, 0, 1)));
		transformA->position = glm::vec3(-1.05f, 1.0f, 0);
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		transformB->position = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		meshA = (MeshComponent*)cubeA->GetComponent("Mesh");
		meshB = (MeshComponent*)cubeB->GetComponent("Mesh");
	}

	void Update()
	{
		value += 0.01f;

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		transformA->Rotate(glm::angleAxis(0.02f, glm::vec3(0, 0, 1)));
		transformA->position = glm::vec3(0.5*sin(1.5*value) - 2.1, 1.0, 0);
		transformB->Rotate(glm::angleAxis(0.015f, glm::vec3(0.2, 0.3, 0.55)));

		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		for (int i = 0; i < 8/*!!!*/; ++i)
			pointsA[i] = modelA * boxPoints[i];
		for (int i = 0; i < 8/*!!!*/; ++i)
			pointsB[i] = modelB * boxPoints[i];

		if (BoxesCollideGJK())
		{
			meshA->diffuseColor = glm::vec3(1, 0, 0);
		}
		else
		{
			meshA->diffuseColor = glm::vec3(0, 1, 0);
		}
	}
};