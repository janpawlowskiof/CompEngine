#pragma once

#include "stdafx.h"

#include "master.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class RendererMaster : public Master
{
private:
	GLFWwindow* window;

public:
	RendererMaster();
	~RendererMaster();

	void Initialize();
	void Update();
};

