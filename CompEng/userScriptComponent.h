#pragma once
#include "component.h"
class UserScriptComponent : public Component
{
public:
	UserScriptComponent();
	~UserScriptComponent();

	void Initialize() {};
	void Update() {};

	std::string type() { return "UserScript"; }
};

