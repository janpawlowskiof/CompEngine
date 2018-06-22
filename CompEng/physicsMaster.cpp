#include "stdafx.h"
#include "physicsMaster.h"
#include "colliderComponent.h"
#include "rigidbodyComponent.h"
#include "rendererMaster.h"
#include <random>

#define MAX_EPA_ITERATIONS 32
#define MAX_GJK_ITERATIONS 32

#define PERSISTENT_THREASHOLD 0.1

#define dT 0.01f

extern std::vector<BaseObject*> baseObjectCollection;
extern RendererMaster* rendererMaster;

void PhysicsMaster::Initialize()
{
}
bool skip = false;

void PrintVec3(std::string s, glm::vec3 v)
{
	std::cout << s << " :: " << v.x << " : " << v.y << " : " << v.z << std::endl;
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
	for (BaseObject* baseObject : baseObjectCollection)
	{
		RigidbodyComponent* rigidbody = (RigidbodyComponent*)baseObject->GetComponent("Rigidbody");
		if (rigidbody != NULL)
		{
			rigidbody->Update();
		}
	}
	ProcessObjects2(BaseObject::Find("Cube1"), BaseObject::Find("Cube2"));
}

void PhysicsMaster::ProcessObjects2(BaseObject * objectA, BaseObject * objectB)
{
	BaseObject* sphere1 = BaseObject::Find("Sphere1");
	TransformComponent* sphere1T = (TransformComponent*)sphere1->GetComponent("Transform");
	BaseObject* sphere2 = BaseObject::Find("Sphere2");
	TransformComponent* sphere2T = (TransformComponent*)sphere2->GetComponent("Transform");
	BaseObject* sphere3 = BaseObject::Find("Sphere3");
	TransformComponent* sphere3T = (TransformComponent*)sphere3->GetComponent("Transform");
	BaseObject* sphere4 = BaseObject::Find("Sphere4");
	TransformComponent* sphere4T = (TransformComponent*)sphere4->GetComponent("Transform");
	BaseObject* sphere5 = BaseObject::Find("Sphere5");
	TransformComponent* sphere5T = (TransformComponent*)sphere5->GetComponent("Transform");
	BaseObject* sphere6 = BaseObject::Find("Sphere6");
	TransformComponent* sphere6T = (TransformComponent*)sphere6->GetComponent("Transform");
	BaseObject* sphere7 = BaseObject::Find("Sphere7");
	TransformComponent* sphere7T = (TransformComponent*)sphere7->GetComponent("Transform");
	BaseObject* sphere8 = BaseObject::Find("Sphere8");
	TransformComponent* sphere8T = (TransformComponent*)sphere8->GetComponent("Transform");

	ColliderComponent* colliderA = (ColliderComponent*)(objectA->GetComponent("Collider"));
	ColliderComponent* colliderB = (ColliderComponent*)(objectB->GetComponent("Collider"));
	RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)(objectA->GetComponent("Rigidbody"));
	TransformComponent* transformA = (TransformComponent*)(objectA->GetComponent("Transform"));


	CollisionData collisionData;

	if (GetObjectsCollisionData(colliderA, colliderB, collisionData))
	{
		for (int i = 0; i < contacts.size(); ++i)
		{
			RecalculateActualPenetration(&contacts[i]);

			if (!ContactValid(&contacts[i]))
			{
				contacts.erase(contacts.begin() + i);
				i--;
			}
		}

		if (ContactFarEnough(&collisionData))
			contacts.push_back(collisionData);

		if (contacts.size() > 4)
		{
			for (int i = 0; i < contacts.size(); ++i)
				if (ContactInsideContactFace(i))
				{
					contacts.erase(contacts.begin() + i);
					i--;
				}
		}

		if (contacts.size() > 8)
		{
			contacts.erase(contacts.begin());
		}

		/*sphere1T->position = transformA->LocalToGlobalSpace(contacts[0].localPositionA);
		if (contacts.size() > 1)
			sphere2T->position = transformA->LocalToGlobalSpace(contacts[1].localPositionA);
		else
			sphere2T->position = glm::vec3(0, -100, 0);

		if (contacts.size() > 2)
			sphere3T->position = transformA->LocalToGlobalSpace(contacts[2].localPositionA);
		else
			sphere3T->position = glm::vec3(0, -100, 0);

		if (contacts.size() > 3)
			sphere4T->position = transformA->LocalToGlobalSpace(contacts[3].localPositionA);
		else
			sphere4T->position = glm::vec3(0, -100, 0);

		if (contacts.size() > 4)
			sphere5T->position = transformA->LocalToGlobalSpace(contacts[4].localPositionA);
		else
			sphere5T->position = glm::vec3(0, -100, 0);

		if (contacts.size() > 5)
			sphere6T->position = transformA->LocalToGlobalSpace(contacts[5].localPositionA);
		else
			sphere6T->position = glm::vec3(0, -100, 0);

		if (contacts.size() > 6)
			sphere7T->position = transformA->LocalToGlobalSpace(contacts[6].localPositionA);
		else
			sphere7T->position = glm::vec3(0, -100, 0);

		if (contacts.size() > 7)
			sphere8T->position = transformA->LocalToGlobalSpace(contacts[7].localPositionA);
		else
			sphere8T->position = glm::vec3(0, -100, 0);*/

		std::cout << contacts.size() << " \n";

		linearImpulse = glm::vec3(0);
		angularImpulse = glm::vec3(0);

		for (int i = 0; i < contacts.size(); ++i)
			ProcessContact(&contacts[i]);

		CollisionData middleContact = contacts[contacts.size() - 1];
		for (int i = 0; i < contacts.size(); ++i)
		{
			middleContact.localPositionA += contacts[i].localPositionA;
			middleContact.actualPenetration += contacts[i].actualPenetration;
		}
		middleContact.localPositionA /= (float)contacts.size();
		middleContact.actualPenetration /= (float)contacts.size();

		ProcessContact(&middleContact);

		rigidbodyA->velocity += linearImpulse;
		rigidbodyA->angularVelocity += angularImpulse;

	}
	else
	{
		contacts.clear();
	}
}

void PhysicsMaster::ProcessContact(CollisionData* contact)
{
	ColliderComponent* colliderA = (ColliderComponent*)(contact->colliderA->baseObject->GetComponent("Collider"));
	ColliderComponent* colliderB = (ColliderComponent*)(contact->colliderB->baseObject->GetComponent("Collider"));

	RigidbodyComponent* rigidbodyA = (RigidbodyComponent*)contact->colliderA->baseObject->GetComponent("Rigidbody");
	RigidbodyComponent* rigidbodyB = (RigidbodyComponent*)contact->colliderB->baseObject->GetComponent("Rigidbody");

	TransformComponent* transformA = (TransformComponent*)contact->colliderA->baseObject->GetComponent("Transform");
	TransformComponent* transformB = (TransformComponent*)contact->colliderB->baseObject->GetComponent("Transform");

	float E = (colliderA->elasticity + colliderB->elasticity) / 2.0f;	//sprê¿ystoœæ
	glm::vec3 collisionPoint = transformA->LocalToGlobalSpace(contact->localPositionA);
	glm::vec3 n = contact->normal;

	glm::vec3 rA = collisionPoint - transformA->position;
	glm::vec3 rB = collisionPoint - transformB->position;

	glm::vec3 pA, pB;
	if (rigidbodyA != NULL)
		pA = rigidbodyA->velocity + glm::cross(rigidbodyA->angularVelocity, rA);
	else
		pA = glm::vec3(0);

	if (rigidbodyB != NULL)
		pB = rigidbodyB->velocity + glm::cross(rigidbodyB->angularVelocity, rB);
	else
		pB = glm::vec3(0);

	float inverseMassA;
	glm::mat3 inverseInteriaA;
	if (rigidbodyA != NULL)
	{
		inverseInteriaA = colliderA->GetGlobalInverseInteria();
		inverseMassA = rigidbodyA->GetInverseMass();
	}
	else
	{
		inverseInteriaA = glm::mat3(0);
		inverseMassA = 0;
	}

	float inverseMassB;
	glm::mat3 inverseInteriaB;
	if (rigidbodyB != NULL)
	{
		inverseInteriaB = colliderB->GetGlobalInverseInteria();
		inverseMassB = rigidbodyB->GetInverseMass();
	}
	else
	{
		inverseInteriaB = glm::mat3(0);
		inverseMassB = 0;
	}

	float vRel = glm::dot(n, pA - pB) - contact->actualPenetration * 0.2/dT;
	//float vRel = glm::dot(n, pA - pB);

	float j = -(1.0 + E) * vRel;
	j /= inverseMassA + inverseMassB + glm::dot(n, glm::cross(inverseInteriaA * glm::cross(rA, n), rA)) + glm::dot(n, glm::cross(inverseInteriaB * glm::cross(rB, n), rB));
	glm::vec3 J = j * n;

	float f = 0.55f;

	glm::vec3 t1 = contact->tangentA;
	glm::vec3 t2 = contact->tangentB;

	float mT1 = -(1.0 + E) * glm::dot(t1, pA - pB);
	mT1 /= inverseMassA + inverseMassB + glm::dot(t1, glm::cross(inverseInteriaA * glm::cross(rA, t1), rA)) + glm::dot(t1, glm::cross(inverseInteriaB * glm::cross(rB, t1), rB));
	glm::vec3 T1 = mT1 * t1;

	float mT2 = -(1.0 + E) * glm::dot(t2, pA - pB);
	mT2 /= inverseMassA + inverseMassB + glm::dot(t2, glm::cross(inverseInteriaA * glm::cross(rA, t2), rA)) + glm::dot(t2, glm::cross(inverseInteriaB * glm::cross(rB, t2), rB));
	glm::vec3 T2 = mT2 * t2;

	glm::vec3 T;
	glm::vec3 t = pA - pB - glm::dot(n, pA - pB);
	if (t != glm::vec3(0))
	{
		t = glm::normalize(t);
		float mT = -(1.0 + E) * glm::dot(t, pA - pB);
		mT /= inverseMassA + inverseMassB + glm::dot(t, glm::cross(inverseInteriaA * glm::cross(rA, t), rA)) + glm::dot(t, glm::cross(inverseInteriaB * glm::cross(rB, t), rB));
		T = mT * t;

		J += T * f;
	} 
	//J += f * (T1 + T2);
	J /= (float)(contacts.size() + 1);	//cause there are many contacts

	glm::vec3 dVA = J * inverseMassA;	//delta velocity A
	glm::vec3 dVB = J * inverseMassB;	//delta velocity B
	float mdVA = glm::length(dVA);		//magnitude of dVA
	float mdVB = glm::length(dVB);

	if (rigidbodyA != NULL)
	{
		linearImpulse += dVA;
		angularImpulse += inverseInteriaA * glm::cross(rA, J);
		//rigidbodyA->velocity += dVA;

		//rigidbodyA->angularVelocity += inverseInteriaA * glm::cross(rA, J);
		//rigidbodyA->angularVelocity *= 0.9f;	//hack
		//rigidbodyA->Update();
		//transformA->position += 0.5 * n * actualPenetration / (float)contacts.size();
	}

	if (rigidbodyB != NULL)
	{
	 	rigidbodyB->velocity += -dVB;

		rigidbodyB->angularVelocity += -inverseInteriaB * glm::cross(rB, J);
		rigidbodyB->angularVelocity *= 0.9f;	//hack

		//transformB->position += -mdVB / (mdVA + mdVB) * n * contact->penetration/* / (float)contacts.size()*/;
	}
}

void PhysicsMaster::RecalculateActualPenetration(CollisionData * contact)
{
	contact->actualPenetration = contact->penetration + glm::dot(contact->normal, contact->globalPositionA - ((TransformComponent*)contact->colliderA->baseObject->GetComponent("Transform"))->LocalToGlobalSpace(contact->localPositionA));
}

bool PhysicsMaster::ContactValid(CollisionData* contact)
{
	glm::vec3 localToGlobalA = ((TransformComponent*)contact->colliderA->baseObject->GetComponent("Transform"))->LocalToGlobalSpace(contact->localPositionA);
	glm::vec3 localToGlobalB = ((TransformComponent*)contact->colliderB->baseObject->GetComponent("Transform"))->LocalToGlobalSpace(contact->localPositionB);
	glm::vec3 rAB = localToGlobalA - localToGlobalB;
	glm::vec3 rA = contact->globalPositionA - localToGlobalA;
	glm::vec3 rB = contact->globalPositionB - localToGlobalB;

	float mrA = glm::length(rA);
	float mrB = glm::length(rB);
	float penetrating = glm::dot(contact->normal, rAB);

	if (mrA < PERSISTENT_THREASHOLD && mrB < PERSISTENT_THREASHOLD && penetrating < 0.001f)
		return true;
	else
		return false;
}

bool PhysicsMaster::ContactInsideContactFace(int index)
{
	bool outside = true;
	glm::vec3* vectors = new glm::vec3[contacts.size()];


	for (int i = 0; i < contacts.size(); ++i)
	{
		if (index == i)
			continue;
		vectors[i] = contacts[i].localPositionA - contacts[index].localPositionA;
	}

	for (int i = 0; i < contacts.size(); ++i)
	{
		outside = true;
		if (i == index)
			continue;

		glm::vec3 v1 = glm::vec3(0);

		for (int j = 0; j < contacts.size(); ++j)
		{
			if (j == i || j == index)
				continue;

			if (v1 == glm::vec3(0))
			{
				v1 = glm::cross(vectors[i], vectors[j]);
				continue;
			}

			glm::vec3 v2 = glm::cross(vectors[i], vectors[j]);

			if (glm::dot(v1, v2) < 0)
			{
				outside = false;
				break;
			}
		}

		if (outside)
		{
			delete[] vectors;
			return false;
		}
	}


	delete[] vectors;
	return true;
}

bool PhysicsMaster::ContactFarEnough(CollisionData *newContact)
{
	for (CollisionData& contact : contacts)
	{
		glm::vec3 rA = newContact->globalPositionA - contact.globalPositionA;
		glm::vec3 rB = newContact->globalPositionB - contact.globalPositionB;

		if (glm::length(rA) < PERSISTENT_THREASHOLD || glm::length(rB) < PERSISTENT_THREASHOLD)
			return false;
	}
	return true;
}

bool PhysicsMaster::GetObjectsCollisionData(ColliderComponent* colliderA, ColliderComponent* colliderB, CollisionData& collisionData)
{
	glm::vec3 random = glm::vec3((float)(rand() % 2000) / 1000.0f - 1000.0f, (float)(rand() % 2000) / 1000.0f - 1000.0f, (float)(rand() % 2000) / 1000.0f - 1000.0f);
	if (random == glm::vec3(0))
		random = glm::vec3(0, 1, 0);
	random = glm::normalize(random);
	SupportPoint S = Support(colliderA, colliderB, random);	//random dir
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

bool PhysicsMaster::DoSimplex(Simplex& simplex, glm::vec3& D, ColliderComponent* colliderA, ColliderComponent* colliderB, CollisionData& collisionData)
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

CollisionData PhysicsMaster::BuildPolytope(Simplex base, ColliderComponent* colliderA, ColliderComponent* colliderB)
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

			glm::vec3 n = glm::normalize(-faces[closestFaceIndex].normal);	//normalize because of some absud inaccuracy

			glm::vec3 tangentA;
			glm::vec3 tangentB;

			if (n == glm::vec3(1, 0, 0) || n == glm::vec3(-1, 0, 0))
				tangentA = glm::normalize(glm::cross(n, glm::vec3(0, 1, 0)));
			else
				tangentA = glm::normalize(glm::cross(n, glm::vec3(1, 0, 0)));

			tangentB = glm::cross(n, tangentA);

			glm::vec3 localPositionA = ((TransformComponent*)colliderA->baseObject->GetComponent("Transform"))->GlobalToLocalSpace(collisionPointA);
			glm::vec3 localPositionB = ((TransformComponent*)colliderB->baseObject->GetComponent("Transform"))->GlobalToLocalSpace(collisionPointB);

			return CollisionData(collisionPointA, collisionPointB, localPositionA, localPositionB, n, closestDistance, colliderA, colliderB, tangentA, tangentB);
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
