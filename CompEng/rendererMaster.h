#pragma once

#include "stdafx.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "master.h"
#include "shaderSources.h"
#include "shader.h"

class RendererMaster : public Master
{
private:

	Shader* shader;
	GLFWwindow* window;


	/*TESTING TEMP VARIABLES*/

	unsigned int VBO;	//This two will belong to rendererComponent
	unsigned int VAO;

	/*END TESTING TEMP VARIABLES*/

public:
	RendererMaster();
	~RendererMaster();

	void Initialize();
	void Update();
};

