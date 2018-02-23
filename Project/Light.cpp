#include "Light.h"

void Light::initateLights()
{
	//lolbat-deluxe hardcoded colors and positions
	this->light1.direction = { 0.1f, 1.0f, 0.2f, 0.0f};
	this->light1.color = { 1.0f, 1.0f, 1.0f, 5.0f };
	this->light1.camPos = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); //some standard value
}

Light::Light()
{
	this->initateLights();
}

Light::~Light()
{
	//Oh how do you do young Willy McBride do you mind if i sit here down by your graveside and rest for awhile in the warm summer sun - Ive been walking all day and im nearly done - I see by your gravestone you where only nineteen when you joined the great fallen in 1916
}

Light::lights Light::getLights()
{
	return this->light1;
}

void Light::setCamPos(XMFLOAT4 new_pos)
{
	this->light1.camPos = new_pos;
}

