#pragma once
#include "component.h"
#include "Shader.h"

#include <vector>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;

};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class MeshComponent : public Component
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	unsigned int VBO, VAO, EBO;

	MeshComponent(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
	MeshComponent(std::string path);

	void Initialize();
	void Draw(Shader* shader);

	~MeshComponent();
	//MeshComponentComponent();
	glm::vec3 diffuseColor = glm::vec3(1, 1, 0.6);
	glm::vec3 specularColor = glm::vec3(1, 1, 0.6);

	bool useTexture;
	std::string type() { return "Mesh"; }
private:
	bool initialized = false;
};

