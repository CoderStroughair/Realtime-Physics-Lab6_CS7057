#pragma once

#include "SingleMeshLoader.h"
#include "EulerCamera.h"
#include <glm\matrix.hpp>:


vec3 getClosestPointTriangle(vec3 triangle[3], vec3 p0);
vec3 getClosestPointLine(vec3 line[], vec3 p0);
vec3 getClosestPointPlane(vec3 normal, vec3 p1, vec3 p0);
float getDistance(vec3 v0, vec3 v1);
void getClosestTriangleTriangle(vec3 first[], vec3 second[], vec3 &point1, vec3 &point2);

struct Tetrahedral
{
	vector<GLfloat> w = vector<GLfloat>();
	vector<GLfloat> x = vector<GLfloat>();
	vector<GLfloat> y = vector<GLfloat>();
	vector<GLfloat> z = vector<GLfloat>();

	vec3 faceA[3], faceB[3], faceC[3], faceD[3];

	glm::mat4 mInv;	//beta
	glm::mat4 m;

	glm::mat4x3 G = glm::mat4x3(
					1, 0, 0, 0,
					0, 1, 0, 0,
					0, 0, 1, 0);
	glm::mat4x3 GT = glm::mat4x3(
					1, 0, 0,
					0, 1, 0,
					0, 0, 1,
					0, 0, 0);

	glm::mat4x4 stress;
	glm::mat4x4 strain;

	SingleMesh t;

	bool affected(vec3 point, float radius)
	{
		if (getDistance(getClosestPointTriangle(this->faceA, point), point) <= radius)
			return true;
		if (getDistance(getClosestPointTriangle(this->faceB, point), point) <= radius)
			return true;
		if (getDistance(getClosestPointTriangle(this->faceC, point), point) <= radius)
			return true;
		if (getDistance(getClosestPointTriangle(this->faceD, point), point) <= radius)
			return true;

		return false;
	}
};

class TetrahedralisedMesh		//for the moment, I'm going to achieve the required results by adding the origin as a vertex, and then using it as the 4th point for a tetrahedral with all of the faces
{
private:
public:
	SingleMesh base;
	vec3 position;
	vec3 centerNode;
	vector<Tetrahedral> tetrahedrals;
	bool drawTetra = false;
	float collisionRadius = 0.5;
	void CalculateTetrahedrals();



	void BreakOffTetrahedrals();
	TetrahedralisedMesh();
	TetrahedralisedMesh(SingleMesh mesh, vec3 p);
	void ApplyForce(vec3 point, vec3 force);
	void Draw(mat4 model, mat4 view, mat4 proj, GLuint shaderID, EulerCamera cam);
	void update();

	vec4 GetBarycentricCoords(vec3 a, vec3 b, vec3 c, vec3 d, vec3 p);
};



