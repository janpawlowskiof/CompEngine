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

	shader = new Shader("res/basicShader.vs", "res/basicShader.fs");	//Create Basic Shader
	shader->use();

	//Temporary Camera Configuration
	camera = new Camera(glm::vec3(0.0f, 0.0f, 4.0f));
	camera->Initialize(window);
	//camera->LookAt(glm::vec3(0, 0, 0));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	modelMatrixLoc = glGetUniformLocation(shader->ID, "modelMatrix");
	viewMatrixLoc = glGetUniformLocation(shader->ID, "viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(shader->ID, "projectionMatrix");

	projectionMatrix = glm::perspective(glm::radians(60.0f), 800.0f/600.0f, 0.1f, 100.0f);
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


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

	//Very Temporary
	shader->use();
	//glBindVertexArray(VAO);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	//Camera Control
	camera->Update(window);
	viewMatrix = camera->GetViewMatrix();
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	//

	for (BaseObject* baseObject : baseObjectCollection)
	{
		RendererComponent* renderer = (RendererComponent*)(baseObject->GetComponent("Renderer"));
		if (renderer != NULL)
		{
			TransformComponent* transform = (TransformComponent*)(baseObject->GetComponent("Transform"));
			if (transform == NULL)
				std::cout << "null!!" << std::endl;

			modelMatrix = transform->GetModelMatrix();
			glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			renderer->Draw();
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
