#include "stdafx.h"
#include "physicsMaster.h"
#include "colliderComponent.h"
#include "rigidbodyComponent.h"
#include "rendererMaster.h"

#define MAX_EPA_ITERATIONS 32
#define MAX_GJK_ITERATIONS 32

extern std::vector<BaseObject*> baseObjectCollection;
extern RendererMaster* rendererMaster;

void PhysicsMaster::Initialize()
{
}
bool skip = false;

void PrintVec3(glm::vec3 v)
{
	std::cout << v.x << " : " << v.y << " : " << v.z << std::endl;
}

void PhysicsMaster::Update()
{
	if (glfwGetKey(rendererMaster->window, GLFW_KEY_U) == 1)
	{
		Sleep(200);
	}

	if (skip)
		return;

	//testing
	ColliderComponent* colliderA = (ColliderComponent*)(BaseObject::Find("Cube1")->GetComponent("Collider"));
	ColliderComponent* colliderB = (ColliderComponent*)(BaseObject::Find("Cube2")->GetComponent("Collider"));

	CollsionData collisionData;
	if (GetObjectsCollsionData(colliderA, colliderB, collisionData))
	{
		//skip = true;
		RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)colliderA->baseObject->GetComponent("Rigidbody");
		RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)colliderB->baseObject->GetComponent("Rigidbody");

		TransformComponent* transformA = (TransformComponent*)colliderA->baseObject->GetComponent("Transform");
		TransformComponent* transformB = (TransformComponent*)colliderB->baseObject->GetComponent("Transform");

		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		float E = 0.2f;	//sprê¿ystoœæ

		glm::vec3 collisionPoint = glm::vec3(0);
		glm::vec3 n = collisionData.normal;

		collisionData.localPositionA = glm::inverse(modelA) * glm::vec4(collisionData.contactPoint, 1.0);
		//collisionPoint = transformA->GetModelMatrix() * glm::vec4(collisionData.localPositionA, 1.0);

		/*for (int i = 0; i < collisions.size(); ++i)
		{
			if (collisions[i].localPositionA == collisionData.localPositionA)
			{
				collisions.erase(collisions.begin() + i);
				i--;
			}
		}*/

		collisions.push_back(collisionData);
		if (collisions.size() > 1)
			collisions.erase(collisions.begin());

		for (int i = 0; i < collisions.size(); ++i)
		{
			glm::vec3 worldPositionA = modelA * glm::vec4(collisions[i].localPositionA, 1.0);
			collisionPoint += worldPositionA;
			//n += collisions[i].normal;
		}
		collisionPoint /= collisions.size();
		//n = glm::normalize(n);

		//if (collisions.size() > 1)
		//	skip = true;

		glm::vec3 rA = collisionPoint - transformA->position;
		glm::vec3 rB = collisionPoint - transformB->position;

		glm::vec3 pA = rigidbodyA->velocity + glm::cross(rigidbodyA->angularVelocity, rA);
		glm::vec3 pB = rigidbodyB->velocity + glm::cross(rigidbodyB->angularVelocity, rB);

		float invIA = 1.0/2.5;
		//float invIB = 1.0/2.5;
		float invIB = 0;

		float vRel = glm::dot(n, pA - pB);

		float j = -(1.0 + E) * vRel;
		j /= 1.0 / rigidbodyA->mass + 1.0 / rigidbodyB->mass + glm::dot(n, glm::cross(invIA * glm::cross(rA, n), rA)) + glm::dot(n, glm::cross(invIB * glm::cross(rB, n), rB));

		glm::vec3 J = j * n;
		glm::vec3 dvA = J / rigidbodyA->mass;
		glm::vec3 dvB = J / rigidbodyB->mass;
		float mdvA = glm::length(dvA);
		float mdvB = glm::length(dvB);

		rigidbodyA->velocity +=  dvA;
		rigidbodyB->velocity += -dvB;

		rigidbodyA->angularVelocity += invIA * glm::cross(rA, J);
		rigidbodyB->angularVelocity += -invIB * glm::cross(rB, J);

		transformA->position += mdvA / (mdvA + mdvB) * n * collisionData.penetration;
		transformB->position += -mdvB / (mdvA + mdvB) * n * collisionData.penetration;
		
		glm::vec3 angularVelocityA = invIA * glm::cross(rA, J);
		glm::quat qx = glm::angleAxis(0.01f * angularVelocityA.x, glm::vec3(1, 0, 0));
		glm::quat qy = glm::angleAxis(0.01f * angularVelocityA.y, glm::vec3(0, 1, 0));
		glm::quat qz = glm::angleAxis(0.01f * angularVelocityA.z, glm::vec3(0, 0, 1));

		transformA->RotateGlobal(qx * qy * qz);

		/*glm::vec3 vab = rigidbodyA->velocity + glm::cross(rigidbodyA->angularVelocity, rA)/(2 * 3.1415) - rigidbodyB->velocity;	//2nd stationary
		//std::cout << "aV " << rigidbodyA->angularVelocity.x << " - " << rigidbodyA->angularVelocity.y << " - " << rigidbodyA->angularVelocity.z << std::endl;
		float vn = glm::dot(vab, collisionData.normal);
		float lJ = (-(1 + E)*vn) / (glm::dot(collisionData.normal, collisionData.normal*(1 / rigidbodyA->mass + 1 / rigidbodyB->mass)));
		rigidbodyA->velocity += collisionData.normal * lJ / rigidbodyA->mass;
		rigidbodyB->velocity += -collisionData.normal * lJ / rigidbodyB->mass;

		float iA = 2;	//?
		float iB = 2;	//?

		//float aJ = (-(1 + E)*vn) / 
		//	(glm::dot(collisionData.normal, collisionData.normal*(1 / rigidbodyA->mass + 1 / rigidbodyB->mass)) +
		//		glm::dot(1.0/iA * glm::cross(glm::cross(rA, collisionPoint), rA) + 1.0/iB * glm::cross(glm::cross(rB, collisionPoint), rB), collisionData.normal));

		//rigidbodyA->velocity += collisionData.normal * aJ / rigidbodyA->mass;
		//rigidbodyB->velocity += collisionData.normal * aJ / rigidbodyB->mass;
		//rigidbodyA->angularVelocity += glm::cross(rA, aJ * collisionData.normal) / iA;
		//rigidbodyB->angularVelocity += glm::cross(rB, aJ * collisionData.normal) / iB;

		auto J = (-(1 + E) * vn) / (1 / rigidbodyA->mass + 1 / rigidbodyB->mass + glm::cross(rA, n) / iA + glm::cross(rB, n) / iB);*/
		/*std::cout << "contact: ";
		PrintVec3(collisionPoint);
		std::cout << "pos: ";
		PrintVec3(transformA->position);
		std::cout << "cross: ";
		PrintVec3(glm::cross(rA, J));*/
		BaseObject* s1 = BaseObject::Find("Sphere1");
		TransformComponent* t1 = (TransformComponent*)s1->GetComponent("Transform");
		t1->position = collisionPoint;

		//BaseObject* s2 = BaseObject::Find("Sphere2");
		//TransformComponent* t2 = (TransformComponent*)s2->GetComponent("Transform");
		//t2->position = transformA->position;
	}
	else
	{
		collisions.clear();
	}

	for (BaseObject* baseObject : baseObjectCollection)
	{
		RigidbodyComponent* rigidbody = (RigidbodyComponent*)baseObject->GetComponent("Rigidbody");
		if (rigidbody != NULL)
		{
			rigidbody->Update();
		}
	}
}

bool PhysicsMaster::GetObjectsCollsionData(ColliderComponent* colliderA, ColliderComponent* colliderB, CollsionData& collisionData)
{
	SupportPoint S = Support(colliderA, colliderB, glm::vec3(0, 1, 0));	//random dir
	Simplex simplex(S);
	glm::vec3 D = -S.point;

	int counter = 0;
	while (counter < MAX_GJK_ITERATIONS)	//max iterations
	{
		SupportPoint A = Support(colliderA, colliderB, D);
		if (glm::dot(A.point, D) <= 0)
		{
			return false;
		}
		simplex.Add(A);
		if (DoSimplex(simplex, D, colliderA, colliderB, collisionData))
		{
			return true;
		}
		counter++;
	}
	std::cout << "too many iterations" << std::endl;
	return false;
}

bool PhysicsMaster::DoSimplex(Simplex& simplex, glm::vec3& D, ColliderComponent* colliderA, ColliderComponent* colliderB, CollsionData& collisionData)
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
			D = glm::cross(glm::cross(ab, ao + glm::vec3(1, 0, 0)), ab);

		if (D == glm::vec3(0))		//s.o.s.
			D = glm::cross(glm::cross(ab, ao + glm::vec3(0, 1, 0)), ab);

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

		if (glm::dot(glm::cross(ab, abc), ao) >= 0)
		{
			simplex = Simplex(a, b);
			D = glm::cross(glm::cross(ab, ao), ab);
			return false;
		}

		if (glm::dot(glm::cross(abc, ac), ao) >= 0)
		{
			simplex = Simplex(a, c);
			D = glm::cross(glm::cross(ac, ao), ac);
			return false;
		}

		if (glm::dot(abc, ao) >= 0)
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
			collisionData = BuildPolytope(simplex, colliderA, colliderB);
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

CollsionData PhysicsMaster::BuildPolytope(Simplex base, ColliderComponent* colliderA, ColliderComponent* colliderB)
{
	std::vector<Simplex> faces;
	faces.push_back(Simplex(base.supports[0], base.supports[1], base.supports[2]));
	faces.push_back(Simplex(base.supports[0], base.supports[2], base.supports[3]));
	faces.push_back(Simplex(base.supports[1], base.supports[2], base.supports[3]));
	faces.push_back(Simplex(base.supports[0], base.supports[1], base.supports[3]));

	int counter = 0;
	while (true)
	{
		if (counter > MAX_EPA_ITERATIONS)
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

		SupportPoint A = Support(colliderA, colliderB, faces[closestFaceIndex].normal);
		float distanceOA = abs(glm::dot(A.point, faces[closestFaceIndex].normal));	//distance to our new point

		if (distanceOA - closestDistance < 0.001)	//is our new point far enough?
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

			return CollsionData(collisionPointA, -faces[closestFaceIndex].normal, closestDistance, colliderA, colliderB);
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
}

void PhysicsMaster::Barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float & u, float & v, float & w)
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

void PhysicsMaster::AddEdge(Simplex edge, std::vector<Simplex>& edges)
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

float PhysicsMaster::OriginTringleDistance(Simplex triangle)
{
	return abs(glm::dot(triangle.normal, triangle.supports[0].point));
}

SupportPoint PhysicsMaster::Support(ColliderComponent* colliderA, ColliderComponent* colliderB, glm::vec3 D)
{
	return SupportPoint(colliderA->Support(D), colliderB->Support(-D));
}

PhysicsMaster::PhysicsMaster()
{
}


PhysicsMaster::~PhysicsMaster()
{
}
