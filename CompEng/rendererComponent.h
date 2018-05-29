#pragma once

#include "component.h"
#include "shader.h"
#include "Material.h"

class RendererComponent : public Component
{
private:
	unsigned int loadTexture(char const * path);
public:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int diffuseMap;
	unsigned int specularMap;
	Material material;
	
	std::string type() { return "Renderer"; }
	void Initialize();
	void Draw(Shader*);

	bool useTexture = false;
};

