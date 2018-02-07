#include "Light.h"

void Light::initateLights()
{
	//lolbat-deluxe hardcoded colors and positions
	this->light1.color = { 1, 1, 1, 150 };
	this->light1.position = { 25, 100, 40, 1 };
}

Light::Light()
{
	this->initateLights();
}

Light::~Light()
{
	//Oh how do you do young willy Mc Bride do you mind if i set here down by your graveside and rest for awhile in the warm summer sun - Ive been walking all day and im nearly done - I see by your gravestone you where only nineteen when you joined the great fallen in 1916
}

Light::lights Light::getLights()
{
	return this->light1;
}

