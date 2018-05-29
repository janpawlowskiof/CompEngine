#pragma once

#include "component.h"
#include <vector>
#include <string>

class BaseObject
{
private:

public:
	BaseObject(std::string name = "null");
	~BaseObject();
	std::string name;
	std::vector<Component*> componentCollection; //Vector containing all components attached to the particular baseObject
	Component* GetComponent(std::string);	//Method for gettig component of certain type
	void AddComponent(Component*);

	static BaseObject* Find(std::string);
};

