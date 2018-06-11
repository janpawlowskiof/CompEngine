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
	virtual std::string type() { return "null"; }
	virtual void Initialize() {  }
	virtual void Update()
	{
		std::cout << "Updating default" << type() << std::endl;
	}
	Component();
	virtual ~Component();
};

