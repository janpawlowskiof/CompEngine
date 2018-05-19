// CompEng.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <vector>

#include "rendererMaster.h"
#include "master.h"

std::vector<Master*> masterCollection;	//LIST OF ALL MASTERS
bool closeProgramFlag = false;	//SHOULD PROGRAM EXIT ITS MAIN LOOP?

int main()
{
	//ADDING MASTERS TO MASTER COLLECTION
	masterCollection.push_back(new RendererMaster());

	//CALL INITIALIZE() ON EACH MASTER
	for (Master* m : masterCollection)
	{
		m->Initialize();
	}

	//MAIN LOOP
	while (!closeProgramFlag)
	{
		//CALL UPDATE() ON EACH MASTER
		for (Master* m : masterCollection)
		{
			m->Update();
		}
	}

	system("pause");
    return 0;
}

