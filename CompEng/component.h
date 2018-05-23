#pragma once

#include <string>
#include <iostream>

class Component
{
private:
	//Component* baseObject;
	//Master* master;

public:
	virtual std::string type() { return "null"; }
	virtual void Initialize() { };
	virtual void Update()
	{
		std::cout << "Updating " << type() << std::endl;
	}
	Component();
	virtual ~Component();
};

