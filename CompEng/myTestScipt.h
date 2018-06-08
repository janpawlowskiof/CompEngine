#pragma once

#include "userScriptComponent.h"
#include "transformComponent.h"
#include <iostream>
#include <vector>

extern RendererMaster* rendererMaster;

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

	Simplex() {};

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
		mesh->vertices[0].position = P[0];
		mesh->vertices[1].position = P[1];
		mesh->vertices[2].position = P[2];
		glm::vec3 n = glm::normalize(glm::cross(P[2] - P[0], P[1] - P[0]));
		mesh->vertices[0].normal = P[0];
		mesh->vertices[1].normal = P[1];
		mesh->vertices[2].normal = P[2];

		mesh->Initialize();

		rendererMaster->currentShader->setMat4("modelMatrix", model);
		mesh->Draw(rendererMaster->currentShader);
		//Sleep(100);
		//glBegin(GL_TRIANGLES);
		/*glVertex3f(P[0].x, P[0].y, P[0].z);
		glVertex3f(P[1].x, P[1].y, P[1].z);
		glVertex3f(P[2].x, P[2].y, P[2].z);

		glEnd();*/
	}
};

class CollsionData
{
public:
	glm::vec3 normal;
	float penetration;

	CollsionData() {};
	CollsionData(glm::vec3 normal, float penetration)
	{
		this->normal = normal;
		this->penetration = penetration;
	}
};

class MyTestScript : public UserScriptComponent
{
public:
	float value = 0;

	BaseObject* cubeA, *cubeB, *cubeC;
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
	glm::vec3 Support(glm::vec3 D)
	{
		//return SupportTEST(D);
		return SupportA(D) - SupportB(-D);
	}
	/*
	glm::vec3 ClosestPointOnLine(glm::vec3 P, glm::vec3 A, glm::vec3 B)
	{
		float t = glm::dot(P - A, glm::normalize(B - A));
		if (t < 0)
			t = 0;
		else if (t > 1)
			t = 1;
		return A + t * glm::normalize(B - A);
	}
	/*
	glm::vec3 ClosestPointOn2DTriangle(glm::vec3 P, Simplex triangle)
	{
		glm::vec3 A = triangle.P[0];
		glm::vec3 B = triangle.P[1];
		glm::vec3 C = triangle.P[2];

		float area = glm::length(glm::cross(B - A, C - A));
		float pointArea = glm::length(glm::cross(C - A, P - A)) +	//unfortunate name
			glm::length(glm::cross(B - C, P - C)) +
			glm::length(glm::cross(A - B, P - B));


		if (area == pointArea)	//is point inside the triangle?
		{
			//inside the triangle
			return P;
			//return (B - A) * w1 + (C - A) * w2;	//im tired
		}
		else
		{
			//outside the triangle
			//AB
			glm::vec3 closestPoint = ClosestPointOnLine(P, A, B);
			float minDistance = glm::length(P - closestPoint);
			//BC
			glm::vec3 closestOnCurrent = ClosestPointOnLine(P, B, C);
			float distanceOnCurrent = glm::length(P - closestOnCurrent);
			if (distanceOnCurrent < minDistance)
			{
				minDistance = distanceOnCurrent;
				closestPoint = closestOnCurrent;
			}
			//CA
			closestOnCurrent = ClosestPointOnLine(P, C, A);
			distanceOnCurrent = glm::length(P - closestOnCurrent);
			if (distanceOnCurrent < minDistance)
			{
				minDistance = distanceOnCurrent;
				closestPoint = closestOnCurrent;
			}
			return closestPoint;
		}
	}
	/*
	glm::vec3 ProjectPointOnTrianglePlane(glm::vec3 P, Simplex triangle)
	{
		glm::vec3 N = glm::normalize(glm::cross(triangle.P[1] - triangle.P[0], triangle.P[2] - triangle.P[0]));	//normal
		return P - N * glm::dot(P - triangle.P[0], N);
	}*/

	float OriginTringleDistance(Simplex triangle)
	{
		glm::vec3 N = glm::normalize(glm::cross(triangle.P[1] - triangle.P[0], triangle.P[2] - triangle.P[0]));	//normal
		return abs(glm::dot(N, triangle.P[0]));
	}
	/*
	float PointTriangleDistace(glm::vec3 P, Simplex triangle)
	{
		glm::vec3 projectedPoint = ProjectPointOnTrianglePlane(P, triangle);
		glm::vec3 closestPointOnTriangle = ClosestPointOn2DTriangle(projectedPoint, triangle);
		return glm::length(P - closestPointOnTriangle);
	}
	*/
	void AddEdge(Simplex edge, std::vector<Simplex>& edges)
	{
		for (int e = 0; e < edges.size(); ++e)
		{
			if ((edges[e].P[0] == edge.P[0] && edges[e].P[1] == edge.P[1]) || (edges[e].P[0] == edge.P[1] && edges[e].P[1] == edge.P[0]))
			{
				edges.erase(edges.begin() + e);
				return;
			}
		}
		edges.push_back(edge);
	}

	CollsionData BuildPolytope(Simplex base)
	{
		std::vector<Simplex> faces;
		faces.push_back(Simplex(base.P[0], base.P[1], base.P[2]));
		faces.push_back(Simplex(base.P[0], base.P[2], base.P[3]));
		faces.push_back(Simplex(base.P[1], base.P[2], base.P[3]));
		faces.push_back(Simplex(base.P[0], base.P[1], base.P[3]));

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
			//we now have closest face and its index
			glm::vec3 N = glm::normalize(glm::cross(faces[closestFaceIndex].P[1] - faces[closestFaceIndex].P[0], faces[closestFaceIndex].P[2] - faces[closestFaceIndex].P[0]));	//normal
			if (glm::dot(N, faces[closestFaceIndex].P[1]) < 0)	//normal pointing inwards (we want it outwards)
				N = -N;	//we now have normal
			if (N != N)
				N = glm::vec3(1, 1, 1);

			glm::vec3 A = Support(N);
			float distanceOA = abs(glm::dot(A, N));	//distance to our new point

			if (distanceOA - closestDistance < 0.0001)	//is our new point far enough?
			{
				//it's to close - terminate
				return CollsionData(N, closestDistance);
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
						continue;

					glm::vec3 currentNormal = glm::normalize(glm::cross(faces[i].P[1] - faces[i].P[0], faces[i].P[2] - faces[i].P[0]));
					if (glm::dot(currentNormal, faces[i].P[0]) < 0)	//normal pointing inwards (we want it outwards)
						currentNormal = -currentNormal;	//we now have normal

					if (glm::dot(A - faces[i].P[0], currentNormal) > 0)	//can face be seen?
					{
						//yes
						//add egde to edge if such edgde doesnt exist
						//if said edge already exists than delete both instances
						//Simplex edge;
						AddEdge(Simplex(faces[i].P[0], faces[i].P[1]), edges);

						AddEdge(Simplex(faces[i].P[1], faces[i].P[2]), edges);

						AddEdge(Simplex(faces[i].P[0], faces[i].P[2]), edges);

						faces.erase(faces.begin() + i);
						i--;
					}
				}

				for (int i = 0; i < edges.size(); ++i)
				{
					faces.push_back(Simplex(edges[i].P[0], edges[i].P[1], A));
				}
				edges.clear();

				//add newly created faces
				//faces.push_back(Simplex(faces[closestFaceIndex].P[0], faces[closestFaceIndex].P[1], A));
				//faces.push_back(Simplex(faces[closestFaceIndex].P[1], faces[closestFaceIndex].P[2], A));
				//faces.push_back(Simplex(faces[closestFaceIndex].P[0], faces[closestFaceIndex].P[2], A));

				//delete old one
				//faces.erase(faces.begin() + closestFaceIndex);
			}
		}
		return CollsionData();
	}

	bool DoSimplex(Simplex& simplex, glm::vec3& D, CollsionData& collisionData)
	{
		if (simplex.count == 2)
		{
			glm::vec3 a = simplex.P[1];
			glm::vec3 b = simplex.P[0];

			glm::vec3 ab = b - a;
			glm::vec3 ao = -a;

			simplex = Simplex(a, b);	//????
			D = glm::cross(glm::cross(ab, ao), ab);

			if (D == glm::vec3(0))		//s.o.s.
				D = glm::cross(ab, a + glm::vec3(1));

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
				///
				collisionData = BuildPolytope(simplex);
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

	bool BoxesCollideGJK(CollsionData& collisionData)
	{
		glm::vec3 S = Support(glm::vec3(0, 1, 0));	//random dir
		Simplex simplex(S);
		glm::vec3 D = -S;

		int counter = 0;
		while (counter < 64)	//max iterations
		{
			glm::vec3 A = Support(D);
			if (glm::dot(A, D) < 0)
			{
				//std::cout << counter << std::endl;
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

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		//transformA->Rotate(glm::angleAxis(1.01f, glm::vec3(0, 0, 1)));
		transformA->position = glm::vec3(-1.05f, 1.0f, 0);
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		transformB->position = glm::vec3(0.0f, 1.0f, 0.0f);
		TransformComponent* transformC = (TransformComponent*)cubeC->GetComponent("Transform");
		transformC->position = glm::vec3(0.0f, 4.0f, 0.0f);

		meshA = (MeshComponent*)cubeA->GetComponent("Mesh");
		meshB = (MeshComponent*)cubeB->GetComponent("Mesh");

		//TESTING
		/*Simplex s(glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		//glm::vec3 p = ClosestPointOn2DTriangle(glm::vec3(1, 1, 0), s);
		//glm::vec3 p = ClosestPointOnLine(glm::vec3(1, 1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		auto p = SupportTEST(glm::vec3(0, 1, 0));
		std::cout << "Ans: " << p.x << " - " << p.y << " - " << p.z << std::endl;*/
	}

	void Update()
	{
		float interval = 0.006f;
		value += interval;

		TransformComponent* transformA = (TransformComponent*)cubeA->GetComponent("Transform");
		TransformComponent* transformB = (TransformComponent*)cubeB->GetComponent("Transform");
		TransformComponent* transformC = (TransformComponent*)cubeC->GetComponent("Transform");

		transformA->position += 0.1f * glm::vec3(glfwGetKey(rendererMaster->window, GLFW_KEY_L) - glfwGetKey(rendererMaster->window, GLFW_KEY_J),
			0,
			glfwGetKey(rendererMaster->window, GLFW_KEY_I) - glfwGetKey(rendererMaster->window, GLFW_KEY_K));
		//transformA->Rotate(glm::angleAxis(interval, glm::vec3(0, 0, 1)));

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
		}
		else
		{
			meshA->diffuseColor = glm::vec3(0, 1, 0);
		}
		transformC->rotation = glm::lookAt(glm::vec3(0), collisionData.normal, glm::vec3(0, 1, 0));
	}
};