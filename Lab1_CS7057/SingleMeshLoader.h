#pragma once
#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define CUBEMAP_RIGHT	"-posx.png"
#define CUBEMAP_LEFT	"-negx.png"
#define CUBEMAP_TOP		"-posy.png"
#define CUBEMAP_BOT		"-negy.png"
#define CUBEMAP_FRONT	"-posz.png"
#define CUBEMAP_BACK	"-negz.png"
#define TEXTURE_FOLDER "../Textures/"

#pragma region MESH
/**
Please Note: Objects with multiple meshes don't work in this. Believe me, I've tried, and failed, and failed again. So make your object all one mesh.
Requirements:
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your Init Function:
torch_mesh.init(shaderProgramID, TORCH_MESH, torch_file);
TORCH_MESH is defined at the start of main.cpp as the location of the object. torch_file is a char* holding the location of the texture file.
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your Display() Function:
torch_mesh.activate();
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Many parts of this region were created alongside the learnopengl.com tutorials
**/

class SingleMesh {

public:
	SingleMesh();
	//loading and binding vao and tex
	bool SingleMesh::load_mesh(const char* file_name);
	bool load_texture(const char* file_name, GLuint* tex);

	//whats called from main
	void init(const char* mesh_file, const char* tex_file, const char* normal_file);
	void initCubeMap(GLfloat vertices[], int num_vertices, string texture);
	void SingleMesh::loadCubeFace(GLuint textureID, GLenum side, const char* filename);
	bool SingleMesh::update_mesh(mat4 orientation, vec3 position);
	bool SingleMesh::scale_mesh(GLfloat scale);
	GLuint VAO[20], tex, norm;
	int mesh_vertex_count, mesh_indice_count;
	bool UVmap = false;
	vector<GLuint> indices;


	vector<GLfloat> newpoints; // array of vertex points
	vector<GLfloat> newnormals; // array of vertex normals
	bool converted = false;
	vector<GLfloat> initialpoints;
	vector<GLfloat> initialnormals;
	vector<GLfloat> initialtexCoords;


	//The following elements are only used for indexed meshes

	void convertMesh();
};

#pragma endregion
