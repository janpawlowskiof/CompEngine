#include "stdafx.h"
#include "userScriptMaster.h"

extern std::vector<BaseObject*> baseObjectCollection;

void UserScriptMaster::Initialize()
{
	for (BaseObject* baseObject : baseObjectCollection)
	{
		UserScriptComponent* script = baseObject->GetComponent<UserScriptComponent>();
		if (script != NULL)
		{
			script->Initialize();
		}
	}
}

void UserScriptMaster::Update()
{
	for (BaseObject* baseObject : baseObjectCollection)
	{
		UserScriptComponent* script = baseObject->GetComponent<UserScriptComponent>();
		if (script != NULL)
		{
			script->Update();
		}
	}
}

UserScriptMaster::UserScriptMaster()
{
}

UserScriptMaster::~UserScriptMaster()
{
}

