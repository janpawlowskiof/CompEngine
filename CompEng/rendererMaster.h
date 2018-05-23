#pragma once

#include "stdafx.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "master.h"
#include "shaderSources.h"
#include "shader.h"
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RendererMaster : public Master
{
private:

	Shader* shader;
	GLFWwindow* window;


	/*TESTING TEMP VARIABLES*/

	unsigned int VBO;	//These all will belong to rendererComponent
	unsigned int VAO;
	unsigned int texture;

	//Matrix - Camera Stuff
	glm::mat4 modelMatrix;	//Model
	glm::mat4 viewMatrix;	//View
	glm::mat4 projectionMatrix; //Projection

	unsigned int modelMatrixLoc;	//Locations
	unsigned int viewMatrixLoc;
	unsigned int projectionMatrixLoc;

	/*END TESTING TEMP VARIABLES*/

public:
	RendererMaster();
	~RendererMaster();

	void Initialize();
	void Update();
};

