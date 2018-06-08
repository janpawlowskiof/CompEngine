#pragma once

#include "userScriptComponent.h"
#include "transformComponent.h"
#include <iostream>
#include <vector>

extern RendererMaster* rendererMaster;

struct SupportPoint
{
	glm::vec3 point;	//point on minkowski diffrence
	glm::vec3 supportA;	//point of SuppportA - used to get collision point
	glm::vec3 supportB;

	SupportPoint(glm::vec3 p, glm::vec3 sA, glm::vec3 sB)
	{
		point = p;
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
		normal = glm::normalize(glm::cross(supports[1].point - supports[0].point, supports[2].point - supports[0].point));	//normal
		if (glm::dot(normal, supports[1].point) < 0)	//normal pointing inwards (we want it outwards)
			normal = -normal;	//we now have normal
		if (normal.x != normal.x)	//NaN error, objects probably touching
			normal = glm::vec3(1, 1, 1);
	}

	//das broken :c
	void DebugDraw(glm::vec3 color)
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
	}
};

class CollsionData
{
public:
	glm::vec3 contactPoint;
	glm::vec3 normal;
	float penetration;

	CollsionData() {};
	CollsionData(glm::vec3 contactPoint, glm::vec3 normal, float penetration)
	{
		this->contactPoint = contactPoint;
		this->normal = normal;
		this->penetration = penetration;
	}
};

class MyTestScript : public UserScriptComponent
{
public:
	float value = 0;

	BaseObject *cubeA, *cubeB, *cubeC, *sphereA;
	MeshComponent* meshA, *meshB, *meshC;

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
	glm::vec3 SupportTEST(glm::vec3 D)
	{
		glm::vec3 furthestPoint = boxPoints[0];
		float furthestDistance = glm::dot(D, (glm::vec3)boxPoints[0]);
		for (int i = 1; i < 8/*!!!*/; ++i)
		{
			float distance = glm::dot(D, (glm::vec3)boxPoints[i]);
			if (distance > furthestDistance)
			{
				furthestDistance = distance;
				furthestPoint = (glm::vec3)boxPoints[i];
			}
		}
		return furthestPoint;
	}
	SupportPoint Support(glm::vec3 D)
	{
		auto supportA = SupportA(D);
		auto supportB = SupportB(-D);
		return SupportPoint(supportA - supportB, supportA, supportB);
	}

	float OriginTringleDistance(Simplex triangle)
	{
		glm::vec3 N = glm::normalize(glm::cross(triangle.supports[1].point - triangle.supports[0].point, triangle.supports[2].point - triangle.supports[0].point));	//normal
		return abs(glm::dot(N, triangle.supports[0].point));
	}

	void AddEdge(Simplex edge, std::vector<Simplex>& edges)
	{
		for (int e = 0; e < edges.size(); ++e)
		{
			if ((edges[e].supports[0].point == edge.supports[0].point && edges[e].supports[1].point == edge.supports[1].point) || (edges[e].supports[0].point == edge.supports[1].point && edges[e].supports[1].point == edge.supports[0].point))
			{
				edges.erase(edges.begin() + e);
				return;
			}
		}
		edges.push_back(edge);
	}

	//Christer Ericson's Real-Time Collision Detection
	void Barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float &u, float &v, float &w)
	{
		glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = glm::dot(v0, v0);
		float d01 = glm::dot(v0, v1);
		float d11 = glm::dot(v1, v1);
		float d20 = glm::dot(v2, v0);
		float d21 = glm::dot(v2, v1);
		float denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;
		u = 1.0f - v - w;
	}

	CollsionData BuildPolytope(Simplex base)
	{
		std::vector<Simplex> faces;
		faces.push_back(Simplex(base.supports[0], base.supports[1], base.supports[2]));
		faces.push_back(Simplex(base.supports[0], base.supports[2], base.supports[3]));
		faces.push_back(Simplex(base.supports[1], base.supports[2], base.supports[3]));
		faces.push_back(Simplex(base.supports[0], base.supports[1], base.supports[3]));

		int counter = 0;
		while (true)
		{
			if (counter > 32)
				std::cout << "Fatal EPA error \n";

			counter++;


			//Getting closest face
			float closestDistance = OriginTringleDistance(faces[0]);
			int closestFaceIndex = 0;
			for (int i = 1; i < faces.size(); ++i)
			{
				float distance = OriginTringleDistance(faces[i]);
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestFaceIndex = i;
				}
			}

			//N = faces[closestFaceIndex].normal
			SupportPoint A = Support(faces[closestFaceIndex].normal);
			float distanceOA = abs(glm::dot(A.point, faces[closestFaceIndex].normal));	//distance to our new point

			if (distanceOA - closestDistance < 0.0001)	//is our new point far enough?
			{
				//it's to close - we found our (almost?) closest triangle
				float barycentricU, barycentricV, barycentricW;

				Barycentric(faces[closestFaceIndex].normal * closestDistance,
					faces[closestFaceIndex].supports[0].point,
					faces[closestFaceIndex].supports[1].point,
					faces[closestFaceIndex].supports[2].point,
					barycentricU, barycentricV, barycentricW);
				glm::vec3 collisionPointA = faces[closestFaceIndex].supports[0].supportA * barycentricU +
					faces[closestFaceIndex].supports[1].supportA * barycentricV +
					faces[closestFaceIndex].supports[2].supportA * barycentricW;

				glm::vec3 collisionPointB = faces[closestFaceIndex].supports[0].supportB * barycentricU +
					faces[closestFaceIndex].supports[1].supportB * barycentricV +
					faces[closestFaceIndex].supports[2].supportB * barycentricW;

				return CollsionData((collisionPointA + collisionPointB)/2.0f, -faces[closestFaceIndex].normal, closestDistance);
			}
			else
			{
				/*if (counter > -1)								//DEBUG DRAWING
				{
					bool t = false;
					while (!t)
					{
						///llllllllllllllllllllllllllllllllllll
						t = glfwGetKey(rendererMaster->window, GLFW_KEY_P);
						if (t)
							Sleep(200);

						if (glfwGetKey(rendererMaster->window, GLFW_KEY_B))
						{
							std::cout << "b \n";
						}
						
						glClear(GL_COLOR_BUFFER_BIT);
						glClear(GL_DEPTH_BUFFER_BIT);

						Simplex o(glm::vec3(-0.1, 0, 0), glm::vec3(0, 0.1, 0), glm::vec3(0.1, 0, 0));
						o.DebugDraw(glm::vec3(0,0,1));

						Simplex a(A + glm::vec3(-0.1, 0, 0), A + glm::vec3(0, 0.1, 0), A + glm::vec3(0.1, 0, 0));
						a.DebugDraw(glm::vec3(0, 1, 1));

						Simplex n(N + A + glm::vec3(-0.1, 0, 0), N + A + glm::vec3(0, 0.1, 0), N + A + glm::vec3(0.1, 0, 0));
						n.DebugDraw(glm::vec3(1, 0, 1));

						for (int i = 0; i < faces.size(); ++i)
						{
							if(i == closestFaceIndex)
								faces[i].DebugDraw(glm::vec3(1, 0, 0));
							else
								faces[i].DebugDraw(glm::vec3(0, 0.6 + 0.05*i, 0));
						}
						rendererMaster->camera->Update(rendererMaster->window);
						rendererMaster->currentShader->setMat4("viewMatrix", rendererMaster->camera->GetViewMatrix());
						glfwSwapBuffers(rendererMaster->window);
						glfwPollEvents();
					}
				}*/

				//yes
				//these will be turend into triangles
				std::vector<Simplex> edges;

				//erase every face that can be seen
				for (int i = 0; i < faces.size(); ++i)
				{
					if (faces[i].count == 2)
						continue;	//just a failsafe

					if (glm::dot(A.point - faces[i].supports[0].point, faces[i].normal) > 0)	//can face be seen?
					{
						//yes
						//add egde to edge if such edgde doesnt exist
						//if said edge already exists than delete both instances
						AddEdge(Simplex(faces[i].supports[0], faces[i].supports[1]), edges);
						AddEdge(Simplex(faces[i].supports[1], faces[i].supports[2]), edges);
						AddEdge(Simplex(faces[i].supports[0], faces[i].supports[2]), edges);

						faces.erase(faces.begin() + i);
						i--;
					}
				}

				for (int i = 0; i < edges.size(); ++i)
				{
					faces.push_back(Simplex(edges[i].supports[0], edges[i].supports[1], A));
				}
				edges.clear();
			}
		}
		return CollsionData();
	}

	bool DoSimplex(Simplex& simplex, glm::vec3& D, CollsionData& collisionData)
	{
		if (simplex.count == 2)
		{
			SupportPoint a = simplex.supports[1];
			SupportPoint b = simplex.supports[0];

			glm::vec3 ab = b.point - a.point;
			glm::vec3 ao = -a.point;

			simplex = Simplex(a, b);	//????
			D = glm::cross(glm::cross(ab, ao), ab);

			if (D == glm::vec3(0))		//s.o.s.
				D = glm::cross(ab, a.point + glm::vec3(1));

			return false;
		}
		else if (simplex.count == 3)
		{
			SupportPoint a = simplex.supports[2];
			SupportPoint b = simplex.supports[1];
			SupportPoint c = simplex.supports[0];

			glm::vec3 ao = -a.point;

			glm::vec3 ab = b.point - a.point;
			glm::vec3 ac = c.point - a.point;
			
			glm::vec3 abc = glm::cross(ab, ac);	//normal

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
		}
		else if (simplex.count == 4)
		{
			SupportPoint a = simplex.supports[3];
			SupportPoint b = simplex.supports[2];
			SupportPoint c = simplex.supports[1];
			SupportPoint d = simplex.supports[0];
			glm::vec3 ao = -a.point;

			glm::vec3 ab = b.point - a.point;
			glm::vec3 ac = c.point - a.point;
			glm::vec3 ad = d.point - a.point;

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
				collisionData = BuildPolytope(simplex);
				return true;	//NICEEEEEEEE
			}

			//What to do with our selected face
			a = simplex.supports[2];
			b = simplex.supports[1];
			c = simplex.supports[0];
			ab = b.point - a.point;
			ac = c.point - a.point;

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

	bool BoxesCollideGJK(CollsionData& collisionData)
	{
		SupportPoint S = Support(glm::vec3(0, 1, 0));	//random dir
		Simplex simplex(S);
		glm::vec3 D = -S.point;

		int counter = 0;
		while (counter < 64)	//max iterations
		{
			SupportPoint A = Support(D);
			if (glm::dot(A.point, D) < 0)
			{
				return false;
			}
			simplex.Add(A);
			if (DoSimplex(simplex, D, collisionData))
			{
				//std::cout << counter << std::endl;
				return true;
			}
			counter++;
		}
		std::cout << "too many iterations" << std::endl;
		return false;
	}

	void Initialize()
	{
		cubeA = BaseObject::Find("Cube1");
		cubeB = BaseObject::Find("Cube2");
		cubeC = BaseObject::Find("Cube3");
		sphereA = BaseObject::Find("Sphere1");

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		transformA->position = glm::vec3(-2.35f, 1.0f, 0);
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		transformB->position = glm::vec3(0.0f, 1.0f, 0.0f);
		TransformComponent* transformC = (TransformComponent*)cubeC->GetComponent("Transform");
		transformC->position = glm::vec3(0.0f, 4.0f, 0.0f);

		transformA->Rotate(glm::angleAxis(0.4f, glm::vec3(0, 0, 1)));
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
		TransformComponent* transformSphere = (TransformComponent*)sphereA->GetComponent("Transform");

		transformA->position += 0.1f * glm::vec3(glfwGetKey(rendererMaster->window, GLFW_KEY_L) - glfwGetKey(rendererMaster->window, GLFW_KEY_J),
			glfwGetKey(rendererMaster->window, GLFW_KEY_U) - glfwGetKey(rendererMaster->window, GLFW_KEY_O),
			glfwGetKey(rendererMaster->window, GLFW_KEY_K) - glfwGetKey(rendererMaster->window, GLFW_KEY_I));
		//transformA->Rotate(glm::angleAxis(1.4f * interval, glm::vec3(0, 0, 1)));
		//transformB->Rotate(glm::angleAxis(0.7f * interval, glm::vec3(-0.3, 0.1, 0.3)));

		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		for (int i = 0; i < 8/*!!!*/; ++i)
			pointsA[i] = modelA * boxPoints[i];
		for (int i = 0; i < 8/*!!!*/; ++i)
			pointsB[i] = modelB * boxPoints[i];

		CollsionData collisionData;
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
		transformC->rotation = glm::lookAt(glm::vec3(0), collisionData.normal, glm::vec3(0, 1, 0));
	}
};