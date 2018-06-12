#include "stdafx.h"
#include "physicsMaster.h"
#include "colliderComponent.h"
#include "rigidbodyComponent.h"
#include "rendererMaster.h"

#define MAX_EPA_ITERATIONS 32
#define MAX_GJK_ITERATIONS 64

extern std::vector<BaseObject*> baseObjectCollection;
extern RendererMaster* rendererMaster;

void PhysicsMaster::Initialize()
{
}
bool skip = false;

void PrintVec3(std::string s, glm::vec3 v)
{
	std::cout << s << " :: " <<  v.x << " : " << v.y << " : " << v.z << std::endl;
}

bool AlmostEqual(glm::vec3 &a, glm::vec3 &b, float margin)
{
	if (abs(a.x - b.x) > margin)
		return false;
	if (abs(a.y - b.y) > margin)
		return false;
	if (abs(a.z - b.z) > margin)
		return false;
	return true;
}

void PhysicsMaster::Update()
{
	ProcessObjects(BaseObject::Find("Cube1"), BaseObject::Find("Cube2"));

	for (BaseObject* baseObject : baseObjectCollection)
	{
		RigidbodyComponent* rigidbody = (RigidbodyComponent*)baseObject->GetComponent("Rigidbody");
		if (rigidbody != NULL)
		{
			rigidbody->Update();
		}
	}
}

void PhysicsMaster::ProcessObjects(BaseObject * objectA, BaseObject * objectB)
{
	ColliderComponent* colliderA = (ColliderComponent*)(objectA->GetComponent("Collider"));
	ColliderComponent* colliderB = (ColliderComponent*)(objectB->GetComponent("Collider"));
	
	RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)objectA->GetComponent("Rigidbody");
	RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)objectB->GetComponent("Rigidbody");

	TransformComponent* transformA = (TransformComponent*)objectA->GetComponent("Transform");
	TransformComponent* transformB = (TransformComponent*)objectB->GetComponent("Transform");

	CollsionData collisionData;

	if (GetObjectsCollsionData(colliderA, colliderB, collisionData))
	{
		glm::mat4 modelA = transformA->GetModelMatrix();
		glm::mat4 modelB = transformB->GetModelMatrix();

		float E = (colliderA->elasticity + colliderB->elasticity)/2.0f;	//sprê¿ystoœæ

		/*absolutely incorrect testing caching contact points*/
		/*glm::vec3 collisionPoint = glm::vec3(0);
		glm::vec3 n = collisionData.normal;

		collisionData.localPositionA = glm::inverse(modelA) * glm::vec4(collisionData.contactPoint, 1.0);

		for (int i = 0; i < collisions.size(); ++i)
		{
			if (AlmostEqual(collisions[i].localPositionA, collisionData.localPositionA, 0.01f))
			{
				collisions.erase(collisions.begin() + i);
				i--;
			}
		}

		collisions.push_back(collisionData);
		if (collisions.size() > 1)
			collisions.erase(collisions.begin());

		for (int i = 0; i < collisions.size(); ++i)
		{
			glm::vec3 worldPositionA = modelA * glm::vec4(collisions[i].localPositionA, 1.0);
			collisionPoint += worldPositionA;
		}
		collisionPoint /= collisions.size();*/
		/*end of contact caching*/

		/*alternative to above*/
		glm::vec3 collisionPoint = collisionData.contactPoint;
		glm::vec3 n = collisionData.normal;
		/*end of alternative*/

		glm::vec3 rA = collisionPoint - transformA->position;
		glm::vec3 rB = collisionPoint - transformB->position;

		glm::vec3 pA = rigidbodyA->velocity + glm::cross(rigidbodyA->angularVelocity, rA);
		glm::vec3 pB = rigidbodyB->velocity + glm::cross(rigidbodyB->angularVelocity, rB);

		float inverseInteriaA, inverseMassA;
		if (rigidbodyA != NULL)
		{
			inverseInteriaA = rigidbodyA->inverseInteria;
			inverseMassA = rigidbodyA->inverseMass;
		}
		else
		{
			inverseInteriaA = 0;
			inverseMassA = 0;
		}

		float inverseInteriaB, inverseMassB;
		if (rigidbodyA != NULL)
		{
			inverseInteriaB = rigidbodyB->inverseInteria;
			inverseMassB = rigidbodyB->inverseMass;
		}
		else
		{
			inverseInteriaB = 0;
			inverseMassB = 0;
		}

		float vRel = glm::dot(n, pA - pB);

		float j = -(1.0 + E) * vRel;
		j /= inverseMassA + inverseMassB + glm::dot(n, glm::cross(inverseInteriaA * glm::cross(rA, n), rA)) + glm::dot(n, glm::cross(inverseInteriaB * glm::cross(rB, n), rB));
		glm::vec3 J = j * n;

		float f = 0.15f;
		glm::vec3 T1 = j * f * glm::dot(collisionData.tangentA, pA - pB) * collisionData.tangentA;
		glm::vec3 T2 = j * f * glm::dot(collisionData.tangentB, pA - pB) * collisionData.tangentB;

		J -= T1 + T2;

		glm::vec3 dvA = J * inverseMassA;
		glm::vec3 dvB = J * inverseMassB;
		float mdvA = glm::length(dvA);
		float mdvB = glm::length(dvB);

		rigidbodyA->velocity += dvA;
		rigidbodyB->velocity += -dvB;

		rigidbodyA->angularVelocity += glm::cross(rA, J) * inverseInteriaA;
		rigidbodyB->angularVelocity += -glm::cross(rB, J) * inverseInteriaB;

		//separating objects
		transformA->position += mdvA / (mdvA + mdvB) * n * collisionData.penetration;
		transformB->position += -mdvB / (mdvA + mdvB) * n * collisionData.penetration;

		glm::vec3 angleChangeA = inverseInteriaA * glm::cross(rA, J);
		glm::quat qx = glm::angleAxis(0.02f * angleChangeA.x, glm::vec3(1, 0, 0));
		glm::quat qy = glm::angleAxis(0.02f * angleChangeA.y, glm::vec3(0, 1, 0));
		glm::quat qz = glm::angleAxis(0.02f * angleChangeA.z, glm::vec3(0, 0, 1));
		transformA->RotateGlobal(qx * qy * qz);

		glm::vec3 angleChangeB = -inverseInteriaB * glm::cross(rB, J);
		qx = glm::angleAxis(0.02f * angleChangeA.x, glm::vec3(1, 0, 0));
		qy = glm::angleAxis(0.02f * angleChangeA.y, glm::vec3(0, 1, 0));
		qz = glm::angleAxis(0.02f * angleChangeA.z, glm::vec3(0, 0, 1));
		transformB->RotateGlobal(qx * qy * qz);

		//BaseObject* s1 = BaseObject::Find("Sphere1");
		//TransformComponent* tr1 = (TransformComponent*)s1->GetComponent("Transform");
	}
	else
	{
		collisions.clear();
	}
}

bool PhysicsMaster::GetObjectsCollsionData(ColliderComponent* colliderA, ColliderComponent* colliderB, CollsionData& collisionData)
{
	SupportPoint S = Support(colliderA, colliderB, glm::vec3(0,1,0));	//random dir
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

		//simplex = Simplex(a, b);	//????
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
		simplex = Simplex(a, c, b);
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

		if (glm::dot(glm::cross(ab, ac), ao) >= 0)
		{
			simplex = Simplex(a, b, c);
		}
		else if (glm::dot(glm::cross(ac, ad), ao) >= 0)
		{
			simplex = Simplex(a, c, d);
		}
		else if (glm::dot(glm::cross(ad, ab), ao) >= 0)
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

			glm::vec3 n = glm::normalize(-faces[closestFaceIndex].normal);

			glm::vec3 tangentA;
			glm::vec3 tangentB;

			//if (n.x > 0.57735f)
			//	tangentA = glm::vec3(n.y, -n.x, 0.0f);
			//else
			//	tangentA = glm::vec3(0, n.z, -n.y);
			if (n == glm::vec3(1, 0, 0) || n == glm::vec3(-1, 0, 0))
				tangentA = glm::normalize(glm::cross(n, glm::vec3(0, 1, 0)));
			else
				tangentA = glm::normalize(glm::cross(n, glm::vec3(1,0,0)));

			tangentB = glm::cross(n, tangentA);
			return CollsionData(collisionPointA, n, closestDistance, colliderA, colliderB, tangentA, tangentB);
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
