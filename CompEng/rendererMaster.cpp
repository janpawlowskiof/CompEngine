#include "stdafx.h"
#include "rendererMaster.h"

#define WIDTH 800
#define HEIGHT 600

extern std::vector<BaseObject*> baseObjectCollection;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

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

	basicShader = new Shader("res/basicShader.vs", "res/basicShader.fs");	//Create Basic Shader
	basicShader->use();
	basicShader->setVec3("lightColor", 1,1,1);

	lightingShader = new Shader("res/lightingShader.vs", "res/lightingShader.fs");	//Create Basic Shader
	lightingShader->use();

	lightingShader->setInt("diffuseMap", 0);
	lightingShader->setVec3("lightPos", 0, 0, 0);

	//Temporary Camera Configuration
	camera = new Camera(glm::vec3(0.0f, 0.0f, 4.0f));
	camera->Initialize(window);
	//

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	projectionMatrix = glm::perspective(glm::radians(60.0f), 800.0f/600.0f, 0.1f, 100.0f);

	for (BaseObject* baseObject : baseObjectCollection)
	{
		RendererComponent* renderer = (RendererComponent*)(baseObject->GetComponent("Renderer"));
		if (renderer != NULL)
		{
			renderer->Initialize();
		}
	}
}   

void RendererMaster::Update()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);


	//Camera Control
	camera->Update(window);
	viewMatrix = camera->GetViewMatrix();
	//

	currentShader = basicShader;
	for (BaseObject* baseObject : baseObjectCollection)
	{
		RendererComponent* renderer = (RendererComponent*)(baseObject->GetComponent("Renderer"));
		if (renderer != NULL)
		{
			TransformComponent* transform = (TransformComponent*)(baseObject->GetComponent("Transform"));
			if (transform == NULL)
				std::cout << "null!!" << std::endl;

			modelMatrix = transform->GetModelMatrix();

			currentShader->use();
			currentShader->setMat4("projectionMatrix", projectionMatrix);
			currentShader->setMat4("viewMatrix", viewMatrix);
			currentShader->setMat4("modelMatrix", modelMatrix);
			currentShader->setVec3("cameraPos", camera->position);
			
			//temporar light setup
			glm::vec3 lightColor = glm::vec3(1, 1, 1);
			currentShader->setVec3("light.ambient", lightColor * 0.2f);
			currentShader->setVec3("light.diffuse", lightColor * 0.65f);
			currentShader->setVec3("light.specular", 1, 1, 1);


			renderer->Draw(currentShader);

			currentShader = lightingShader;
		}
	}


	glfwSwapBuffers(window);
	glfwPollEvents();
}

RendererMaster::RendererMaster()
{
}


RendererMaster::~RendererMaster()
{
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	glfwTerminate();
}
