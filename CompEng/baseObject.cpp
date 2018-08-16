#include "stdafx.h"
#include "baseObject.h"

extern std::vector<BaseObject*> baseObjectCollection;

/*Component* BaseObject::GetComponent(std::string type)		//Method for gettig component of certain type
{
	for (Component* component : componentCollection)
	{
		if (component->type() == type)
			return component;
	}
	return NULL;
}*/

void BaseObject::AddComponent(Component* component)	//Method for adding component of certain type
{
	//TODO add check for this component type existing
	component->baseObject = this;
	component->OnAttach();
	componentCollection.push_back(component);
}

BaseObject * BaseObject::Find(std::string name)
{
	for (BaseObject* baseObject : baseObjectCollection)
	{
		if (baseObject->name == name)
			return baseObject;
	}
	return NULL;
}

BaseObject::BaseObject(std::string name)
{
	this->name = name;
}

BaseObject::~BaseObject()
{
	for (Component* component : componentCollection)
		delete  component;
}
