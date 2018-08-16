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
	//Component* GetComponent2(std::string);	//Method for gettig component of certain type
	template <class T> T* GetComponent()
	{
		for (Component* component : componentCollection)
		{
			if (typeid(*component) == typeid(T) || dynamic_cast<T*>(component))
				return (T*)component;
		}
		return NULL;
	}
	void AddComponent(Component*);
	static BaseObject* Find(std::string);
};