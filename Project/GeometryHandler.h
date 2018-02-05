#pragma once
#include"Geometry.h"
#include "Terrain.h"
#include <vector>

/*
Denna klass är tänkt att hantera de olika Object och ljuskällor i scenen
i form av behållare, objekten kallar sin draw() funktion och kan ändra sitt innehåll mm

*/

class GeometryHandler
{

private:
	std::vector<Geometry*> GeometryObjects; //holds Geometry objects
	Terrain* terrainObj;
	RenderEngine* engine;
public:
	GeometryHandler(RenderEngine* engine);
	~GeometryHandler();

	void update();
};