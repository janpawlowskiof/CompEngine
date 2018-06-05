#pragma once
#include "component.h"
#include <glm\ext.hpp>
#include <glm\gtx\string_cast.hpp>

class UserScriptComponent : public Component
{
public:
	UserScriptComponent();
	~UserScriptComponent();

	void Initialize() {};
	void Update() {};

	std::string type() { return "UserScript"; }
};

