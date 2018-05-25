// CompEng.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "rendererMaster.h"
#include "master.h"
#include "baseObject.h"
#include "rendererComponent.h"
#include "transformComponent.h"

extern std::vector<BaseObject*> baseObjectCollection;

std::vector<Master*> masterCollection;	//LIST OF ALL MASTERS
std::vector<BaseObject*> baseObjectCollection;
bool closeProgramFlag = false;	//SHOULD PROGRAM EXIT ITS MAIN LOOP?

int main()
{
	//Objects Initialization
	masterCollection.push_back(new RendererMaster());


	baseObjectCollection.reserve(1024);
	//Adding TestObject
	BaseObject* object;
	Component* component;

	object = new BaseObject();
	component = new RendererComponent();
	((RendererComponent*)component)->useTexture = false;
	object->AddComponent(component);
	object->AddComponent(new TransformComponent(glm::vec3(0.0f, 0.0f, -1.0f)));
	baseObjectCollection.push_back(object);

	object = new BaseObject();
	object->AddComponent(new RendererComponent());
	object->AddComponent(new TransformComponent(glm::vec3(1.2f, -0.2f, -0.5f)));
	baseObjectCollection.push_back(object);
	//End Objects Initialization

	//Setting Context of every component
	for (BaseObject* baseObject : baseObjectCollection)
	{
		for (Component* component : baseObject->componentCollection)
		{
			component->baseObject = baseObject;
		}
	}

	//CALL INITIALIZE() ON EACH MASTER
	for (Master* m : masterCollection)
	{
		m->Initialize();
	}

	//MAIN LOOP
	while (!closeProgramFlag)
	{
		//CALL UPDATE() ON EACH MASTER
		for (Master* m : masterCollection)
		{
			m->Update();
		}
	}

	system("pause");
    return 0;
}

