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
	void Draw(Shader*);

	bool useTexture = true;
	//glm::vec3 color = glm::vec3(0.2, 0.1, 0.85);
	//void Draw();
	//RendererComponent() {};
	//~RendererComponent();
};

