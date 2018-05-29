#pragma once
#include "master.h"
#include "baseObject.h"
#include "userScriptComponent.h"

class UserScriptMaster : public Master
{
public:
	UserScriptMaster();
	~UserScriptMaster();

	void Initialize();
	void Update();
};

