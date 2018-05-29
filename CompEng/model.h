#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "meshComponent.h"
#include "component.h"
#include <vector>
#include <string>
#include <iostream>
#include <string>

class Model : public Component
{
public:
	std::string path;
	Model(char *path)
	{
		this->path = path;
	}
	void Initialize()
	{
		loadModel(path);
	}
	void Draw(Shader* shader);

	~Model();
	std::string type() { return "Model"; }

private:
	std::vector<Texture> textures_loaded;
	std::vector<MeshComponent> meshes;
	std::string directory;
	/*  Functions   */
	unsigned int Model::loadTexture(const char *path, const std::string &directory);
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	MeshComponent processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

