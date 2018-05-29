#include "stdafx.h"
#include "modelMaster.h"

extern std::vector<BaseObject*> baseObjectCollection;

void ModelMaster::Initialize()
{
	for (BaseObject* baseObject : baseObjectCollection)
	{
		Model* model = (Model*)(baseObject->GetComponent("Model"));
		if (model != NULL)
		{
			model->Initialize();
		}
	}
}

ModelMaster::ModelMaster()
{
}


ModelMaster::~ModelMaster()
{
}
