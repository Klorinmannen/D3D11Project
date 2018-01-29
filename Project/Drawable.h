#pragma once
#include"RenderEngine.h"

/*
Interface för objekt som ska kunnas ritas ut
*/

class Drawable
{
public:
	Drawable();
	~Drawable();

protected:
	RenderEngine * renderEngine;
	virtual void Draw() = 0;
};