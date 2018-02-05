#include "GeometryHandler.h"

GeometryHandler::GeometryHandler(RenderEngine * engine)
{
	this->engine = engine;
	this->terrainObj = new Terrain(engine);

}

GeometryHandler::~GeometryHandler()
{
}

void GeometryHandler::update()
{
	this->terrainObj->Draw();
	for (size_t i = 0; i < this->GeometryObjects.size(); i++)
	{
		this->GeometryObjects[i]->Draw();
	}
}
