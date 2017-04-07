#pragma once

#include "SingleMeshLoader.h"
#include "EulerCamera.h"

struct Tetrahedral
{
	vector<GLfloat*> w = vector<GLfloat*>();
	vector<GLfloat*> x = vector<GLfloat*>();
	vector<GLfloat*> y = vector<GLfloat*>();
	vector<GLfloat*> z = vector<GLfloat*>();

	vector<GLfloat*> face_n1 = vector<GLfloat*>();
	vector<GLfloat*> face_n2 = vector<GLfloat*>();
	vector<GLfloat*> face_n3 = vector<GLfloat*>();
	vector<GLfloat*> face_n4 = vector<GLfloat*>();

	SingleMesh t;
};

class TetrahedralisedMesh		//for the moment, I'm going to achieve the required results by adding the origin as a vertex, and then using it as the 4th point for a tetrahedral with all of the faces
{
private:
public:
	SingleMesh base;
	vec3 position;
	vector<Tetrahedral> tetrahedrals;
	bool drawTetra = false;
	
	void CalculateTetrahedrals();



	void BreakOffTetrahedrals();
	TetrahedralisedMesh();
	TetrahedralisedMesh(SingleMesh mesh, vec3 p);
	void ApplyForce(vec3 point, vec3 normal);	//This will only ever be applied to the first model in the list
	void Draw(mat4 model, mat4 view, mat4 proj, GLuint shaderID, EulerCamera cam);
	void update();
};


