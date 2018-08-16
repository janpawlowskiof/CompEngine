#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <glm\ext.hpp>
#include <glm\gtx\string_cast.hpp>

class BaseObject;

class Component
{
private:

public:
	BaseObject* baseObject;
	virtual void Initialize(){}
	virtual void Update() {}
	virtual void OnAttach(){}
	Component();
	virtual ~Component();
};

