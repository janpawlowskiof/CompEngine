#pragma once

#include <iostream>
#include <vector>
#include "baseObject.h"

class Master
{
private:

public:
	Master();
	virtual ~Master();


	virtual void Initialize();
	virtual void Update() {};
};

