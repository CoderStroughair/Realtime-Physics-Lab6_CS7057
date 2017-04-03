#pragma once
#include "Utilities.h"

class Bezier {

public:
	Bezier() {};
	Bezier(vec3 startpoint, vec3 endpoint, vec3 firstcontrol, vec3 secondcontrol);
	vec3 calculatePosition(float t);

private:
	vec3 p0, p1, c0, c1;
};

Bezier::Bezier(vec3 startpoint, vec3 endpoint, vec3 firstcontrol, vec3 secondcontrol)
{
	p0 = startpoint;
	p1 = endpoint;
	c0 = firstcontrol;
	c1 = secondcontrol;
}

vec3 Bezier::calculatePosition(float t)
{
	float minusT = (1 - t);
	float minusT2 = minusT * minusT;
	float minusT3 = minusT2 * minusT;

	float t2 = t * t;
	float t3 = t2 * t;

	float x = (p0.v[0] * minusT3) + (3 * c0.v[0] * minusT2 * t) + (3 * c1.v[0] * minusT * t2) + (p1.v[0] * t3);
	float y = (p0.v[1] * minusT3) + (3 * c0.v[1] * minusT2 * t) + (3 * c1.v[1] * minusT * t2) + (p1.v[1] * t3);
	float z = (p0.v[2] * minusT3) + (3 * c0.v[2] * minusT2 * t) + (3 * c1.v[2] * minusT * t2) + (p1.v[2] * t3);

	return vec3(x, y, z);
}