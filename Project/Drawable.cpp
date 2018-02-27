#include "Drawable.h"

Drawable::Drawable()
{
	this->distance = -1.0f;
}

Drawable::~Drawable()
{
}

bool Drawable::operator < (const Drawable& other) const
{

	return this->distance < other.distance;
}

void Drawable::distanceToCam(XMFLOAT4 in_camPos)
{
	this->distance = sqrtf(pow(in_camPos.x - midPoint.x, 2) + pow(in_camPos.y - midPoint.y, 2) + pow(in_camPos.z - midPoint.z, 2));
}

bool Drawable::distanceFunc(const Drawable & other) const
{
	return this->distance < other.distance;
}

float Drawable::getDistance() const
{
	return this->distance;
}
