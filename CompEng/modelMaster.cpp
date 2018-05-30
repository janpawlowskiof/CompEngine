#include "stdafx.h"
#include "modelMaster.h"

extern std::vector<BaseObject*> baseObjectCollection;

void ModelMaster::Initialize()
{
	for (BaseObject* baseObject : baseObjectCollection)
	{
		MeshComponent* mesh = (MeshComponent*)(baseObject->GetComponent("Mesh"));
		if (mesh != NULL)
		{
			mesh->Initialize();
		}
	}
}

ModelMaster::ModelMaster()
{
}


ModelMaster::~ModelMaster()
{
}
