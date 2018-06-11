#pragma once

#include "stdafx.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include "master.h"
#include "shaderSources.h"
#include "camera.h"
#include <iostream>
#include <string>
#include "shader.h"
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "baseObject.h"
#include "rendererComponent.h"
#include "transformComponent.h"
#include "lightComponent.h"

class RendererMaster : public Master
{
private:

	Shader* basicShader;
	Shader* lightingShader;

	//Matrix - Camera Stuff
	glm::mat4 modelMatrix;	//Model
	glm::mat4 viewMatrix;	//View
	glm::mat4 projectionMatrix; //Projection

	/*END TESTING TEMP VARIABLES*/

public:
	GLFWwindow * window;	//temp
	Shader* currentShader;
	Camera* camera;

	bool closeProgramFlag;

	RendererMaster();
	~RendererMaster();

	void Initialize();
	void Update();
};

