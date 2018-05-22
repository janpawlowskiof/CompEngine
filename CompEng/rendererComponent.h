#pragma once

#include "component.h"
#include "shader.h"

class RendererComponent : public Component
{
private:

public:
	
	std::string type() { return "Renderer"; }
	RendererComponent();
	~RendererComponent();
};

