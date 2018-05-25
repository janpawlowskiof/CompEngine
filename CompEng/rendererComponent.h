#pragma once

#include "component.h"
#include "shader.h"
#include "Material.h"

class RendererComponent : public Component
{
private:

public:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int texture;
	Material material;
	
	std::string type() { return "Renderer"; }
	void Initialize();
	void Draw(Shader*);

	bool useTexture = false;
	//void Draw();
	//RendererComponent() {};
	//~RendererComponent();
};

