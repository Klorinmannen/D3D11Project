#ifndef DRAWABLE_H
#define DRAWABLE_H
#include"IncludeDX11.h"
#include "Settings.h"
/*
Interface för objekt som ska kunnas ritas ut
*/

class Drawable
{
public:
	Drawable();
	virtual ~Drawable(); //virtual for dynamic_cast
	bool operator < (const Drawable& other) const;

	void distanceToCam(XMFLOAT4 in_camPos);
	bool distanceFunc(const Drawable &other) const;
	float getDistance() const;

protected:
	XMFLOAT4 midPoint;
	float distance;
};

#endif // !DRAWABLE_H