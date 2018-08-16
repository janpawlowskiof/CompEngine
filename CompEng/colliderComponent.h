#pragma once

#include "collisionData.h"
#include "transformComponent.h"
#include "component.h"

class ColliderComponent : public Component
{
public:
	void OnAttach() {}
	ColliderComponent() {}
	~ColliderComponent() {}
};

