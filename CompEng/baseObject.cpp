#include "stdafx.h"
#include "baseObject.h"

Component* BaseObject::GetComponent(std::string type)		//Method for gettig component of certain type
{
	for (Component* component : componentCollection)
	{
		if (component->type() == type)
			return component;
	}
	return NULL;
}

void BaseObject::AddComponent(Component* component)	//Method for adding component of certain type
{
	//TODO add check for this component type existing
	componentCollection.push_back(component);
}

BaseObject::BaseObject()
{
}


BaseObject::~BaseObject()
{
	for (Component* component : componentCollection)
		delete  component;
}
