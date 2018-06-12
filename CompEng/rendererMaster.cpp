#include "stdafx.h"
#include "rendererMaster.h"

#include "model.h"
#include <filesystem>

#define WIDTH 800
#define HEIGHT 600

extern std::vector<BaseObject*> baseObjectCollection;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Model* ourModel;

void RendererMaster::Initialize()
{
	//Initializing OpenGL
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);	//Create Window
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		system("pause");
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	//Method To resize window properly

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	//Load glad
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		system("pause");
	}

	//Enabling OpenGl Features
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	basicShader = new Shader("res/basicShader.vs", "res/basicShader.fs");	//Create Basic Shader
	basicShader->use();
	currentShader = basicShader;
	basicShader->setVec3("lightColor", 1,1,1);

	lightingShader = new Shader("res/lightingShader.vs", "res/lightingShader.fs");	//Create Basic Shader
	lightingShader->use();
	currentShader = lightingShader;

	//Temporary Camera Configuration
	camera = new Camera(glm::vec3(0.0f, 1.0f, 8.0f));
	camera->Initialize(window);
	//

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSwapInterval(1);	//CAP - UNCAP FRAMERATE

	projectionMatrix = glm::perspective(glm::radians(60.0f), (float)WIDTH/HEIGHT, 0.1f, 100.0f);

	for (BaseObject* baseObject : baseObjectCollection)
	{
		RendererComponent* renderer = (RendererComponent*)(baseObject->GetComponent("Renderer"));
		if (renderer != NULL)
		{
		//	renderer->Initialize();
		}
	}

	//ourModel = new Model("D:/Projekty2/VS/CompEng/CompEng/res/nanosuit/nanosuit.obj");;
	//ourModel = new Model("D:/Projekty2/VS/CompEng/CompEng/res/car/car.obj");;
} 

/*void RendererMaster::Update()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}*/

void RendererMaster::Update()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	closeProgramFlag = glfwWindowShouldClose(window);

	//Camera Control
	camera->Update(window);
	viewMatrix = camera->GetViewMatrix();
	//

	//currentShader = lightingShader;

	//Setting up Lights
	int activePointLights = 0;
	for (BaseObject* baseObject : baseObjectCollection)
	{
		LightComponent* light = (LightComponent*)(baseObject->GetComponent("Light"));
		if (light == NULL)
			continue;

		switch (light->lightType)
		{
		case LIGHT_DIRECTIONAL:
			currentShader->setVec3("directionalLight.direction", light->direction);
			currentShader->setVec3("directionalLight.ambient", light->ambient);
			currentShader->setVec3("directionalLight.diffuse", light->diffuse);
			currentShader->setVec3("directionalLight.specular", light->specular);
			break;
		case LIGHT_POINT:
			TransformComponent * transform = (TransformComponent*)(baseObject->GetComponent("Transform"));
			if (transform == NULL)
				std::cout << "POINT LIGHT MUST HAVE A TRANSFORM COMPONENT" << std::endl;
			currentShader->setVec3(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].position"), transform->position);
			currentShader->setVec3(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].ambient"), light->ambient);
			currentShader->setVec3(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].diffuse"), light->diffuse);
			currentShader->setVec3(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].specular"), light->specular);
			currentShader->setFloat(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].constant"), light->constant);
			currentShader->setFloat(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].linear"), light->linear);
			currentShader->setFloat(std::string("pointLights[") + std::to_string(activePointLights) + std::string("].quadratic"), light->quadratic);

			activePointLights++;
			break;
		}
	}
	currentShader->setInt("activePointLights", activePointLights);
	
	//Actual drawing
	for (BaseObject* baseObject : baseObjectCollection)
	{
		MeshComponent* model = (MeshComponent*)(baseObject->GetComponent("Mesh"));
		if (model != NULL)
		{
			TransformComponent* transform = (TransformComponent*)(baseObject->GetComponent("Transform"));
			if (transform == NULL)
				std::cout << "null transform!!" << std::endl;

			modelMatrix = transform->GetModelMatrix();

			currentShader->use();
			currentShader->setMat4("projectionMatrix", projectionMatrix);
			currentShader->setMat4("viewMatrix", viewMatrix);
			currentShader->setMat4("modelMatrix", modelMatrix);
			currentShader->setVec3("cameraPos", camera->position);
			
			model->Draw(currentShader);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
		closeProgramFlag = true;

	glfwSwapBuffers(window);
	glfwPollEvents();

	//glClear(GL_COLOR_BUFFER_BIT);
}

RendererMaster::RendererMaster()
{
}


RendererMaster::~RendererMaster()
{
	//std::cout << "Renderer Master Destructor" << std::endl;
	glDeleteShader(basicShader->ID);
	glDeleteShader(lightingShader->ID);
	glfwTerminate();
}
