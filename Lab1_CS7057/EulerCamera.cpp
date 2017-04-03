#include "EulerCamera.h"

EulerCamera::EulerCamera(vec3 pos, GLfloat y, GLfloat p, GLfloat r)
{
	position = pos;
	front = vec3(0.0, 0.0, 0.0);
	right = vec3(0.0, 0.0, 0.0);
	yaw = y;
	pitch = p;
	roll = r;
	degrees = 0;
	sensitivity = 2.0f;
	changeFront();
}

void EulerCamera::changeFront(GLfloat pi, GLfloat ya, GLfloat ro) {

	pi *= sensitivity;
	ya *= sensitivity;
	ro *= sensitivity;

	pitch += (GLfloat)pi;
	yaw += (GLfloat)ya;
	roll += (GLfloat)ro;
	if (pitch > 89)
		pitch = 89;
	if (pitch < -89)
		pitch = -89;
	if (yaw > 360)
		yaw = 0;
	else if (yaw < 0)
		yaw = 360;

	changeFront();

}

void EulerCamera::changeFront()
{
	GLfloat rpitch = pitch * GLfloat(ONE_DEG_IN_RAD);
	GLfloat ryaw = yaw  * GLfloat(ONE_DEG_IN_RAD);
	GLfloat rroll = roll * GLfloat(ONE_DEG_IN_RAD);
	GLfloat ninety = 90 * GLfloat(ONE_DEG_IN_RAD);

	vec3 f = vec3();
	f.v[0] = cos(ryaw) * cos(rpitch);
	f.v[1] = sin(rpitch);
	f.v[2] = sin(ryaw) * cos(rpitch);
	//get direction to look at and normalise it to make it a unit vector
	front = normalise(f);
	up = vec3(0.0, 1.0, 0.0);
}

void EulerCamera::movForward(GLfloat value) { position += front * value / 10.0f; }

void EulerCamera::movRight(GLfloat value) { position += normalise(cross(front, up))*(value / 10.0f); }

void EulerCamera::move(GLfloat value) { position += vec3(front.v[0] * value / 10.0f, 0.0f, front.v[2] * value / 10.0f); }

void EulerCamera::setPosition(vec3 value) { position = value; }

vec3 EulerCamera::getPosition() { return position; }

vec3 EulerCamera::getFront() { return front; }

void EulerCamera::setFront(vec3 value, GLfloat y, GLfloat p) {
	front = normalise(value);
	yaw = y;
	pitch = p;
}

vec3 EulerCamera::getUp() { return up; }

void EulerCamera::setSensitivity(GLfloat value) { sensitivity = value; }

void EulerCamera::jump(bool& jumping) {
	if (jumping)
	{
		position += vec3(0.0f, 0.3f*cos(degrees * float(ONE_DEG_IN_RAD)), 0.0f);
		front += vec3(0.0f, 0.3f*cos(degrees * float(ONE_DEG_IN_RAD)), 0.0f);
		if (degrees >= 180)
		{
			jumping = false;
			degrees = 0;
		}
		degrees += 5.0f;
	}
	else
		return;
}