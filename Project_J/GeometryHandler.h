#pragma once
#include"Drawable.h"
#include <vector>

/*
Denna klass är tänkt att hantera de olika Object och ljuskällor i scenen
i form av behållare, objekten kallar sin draw() funktion och kan ändra sitt innehåll mm

*/

class GeometryHandler
{

private:
	std::vector<Drawable*> drawableObjects; //holds Geometry objects
	

public:
	GeometryHandler(RenderEngine* engine);
	~GeometryHandler();

};