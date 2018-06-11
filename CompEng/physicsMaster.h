#pragma once
#include "master.h"
#include "collisionData.h"
#include "colliderComponent.h"

class PhysicsMaster : public Master
{
private:
	//values, lists
	std::vector<CollsionData> collisions;

	//methods
	bool GetObjectsCollsionData(ColliderComponent* colliderA, ColliderComponent* colliderB, CollsionData& collisionData);
	bool DoSimplex(Simplex& simplex, glm::vec3& D, ColliderComponent* colliderA, ColliderComponent* colliderB, CollsionData& collisionData);
	CollsionData BuildPolytope(Simplex base, ColliderComponent* colliderA, ColliderComponent* colliderB);
	void Barycentric(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c, float &u, float &v, float &w);
	void AddEdge(Simplex edge, std::vector<Simplex>& edges);
	float OriginTringleDistance(Simplex triangle);
	SupportPoint Support(ColliderComponent* colliderA, ColliderComponent* colliderB, glm::vec3 D);
public:
	void Initialize();
	void Update();

	PhysicsMaster();
	virtual ~PhysicsMaster();
};

