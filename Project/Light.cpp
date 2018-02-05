#include "Light.h"

void Light::initateLights()
{
	//lolbat-deluxe hardcoded colors and positions
	this->lights[0].position = { 10, 10, 10, 1 };
	this->lights[0].color = { 0, 1, 0, 100 };
	this->lights[1].position = { 20, 30, 10, 1 };
	this->lights[1].color = { 1, 0, 0, 60 };
}

Light::Light()
{
	this->initateLights();
}

Light::~Light()
{
	//Oh how do you do young willy Mc Bride do you mind if i set here down by your graveside and rest for awhile in the warm summer sun - Ive been walking all day and im nearly done - I see by your gravestone you where only nineteen when you joined the great fallen in 1916
}

Light::light * Light::getLights()
{
	return this->lights;
}
