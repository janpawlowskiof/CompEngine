#pragma once

#include "component.h"
#include "shader.h"

class RendererComponent : public Component
{
private:

public:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int texture;
	
	std::string type() { return "Renderer"; }
	void Initialize();
	void Draw();
	RendererComponent() {};
	~RendererComponent();
};

