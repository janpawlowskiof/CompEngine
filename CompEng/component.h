#pragma once

#include <string>

class Component
{
public:
	virtual std::string type() { return "null"; }
	Component();
	virtual ~Component();
};

