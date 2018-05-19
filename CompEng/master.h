#pragma once

#include <iostream>

class Master
{
public:
	Master();
	~Master();

	virtual void Initialize() {};
	virtual void Update() {};
};

