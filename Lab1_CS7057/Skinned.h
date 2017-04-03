#pragma once

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

// Assimp includes
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include "Antons_maths_funcs.h"


class SkinnedModel
{
	bool LoadMesh(const char* filename);
	bool processNode(const aiScene* scene, aiNode* node);
	vector<SkinnedMesh> meshes;
	map<string, Bone*> boneMap;
};

bool SkinnedModel::LoadMesh(const char* filename)
{
	const aiScene* scene = aiImportFile(filename, aiProcess_Triangulate | 
													 aiProcess_CalcTangentSpace | 
													 aiProcess_GenSmoothNormals |
													 aiProcess_FlipUVs |
													 aiProcess_JoinIdenticalVertices);
	if (!scene)
	{
		fprintf(stderr, "ERROR: reading mesh %s\n", filename);
		return false;
	}

	processNode(scene, scene->mRootNode);

}

bool SkinnedModel::processNode(const aiScene* scene, aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		SkinnedMesh container;
		container.vertices = NULL;
		container.normals = NULL;
		container.texcoords = NULL;
		container.tangents = NULL;
		container.mesh_vertex_count = mesh->mNumVertices;
		glGenVertexArrays(1, &container.VAO);
		glBindVertexArray(container.VAO);

		if (mesh->HasPositions())
		{
			container.vertices = (GLfloat*)malloc(container.mesh_vertex_count * 3 * sizeof(GLfloat));
			for (int i = 0; i < container.mesh_vertex_count; i++)
			{
				const aiVector3D* vp = &(mesh->mVertices[i]);
				container.vertices[i * 3] = (GLfloat)vp->x;
				container.vertices[i * 3 + 1] = (GLfloat)vp->y;
				container.vertices[i * 3 + 2] = (GLfloat)vp->z;
				container.initial_vertices.push_back(container.vertices[i * 3]);
				container.initial_vertices.push_back(container.vertices[i * 3 + 1]);
				container.initial_vertices.push_back(container.vertices[i * 3 + 2]);
			}
		}
		if (mesh->HasNormals())
		{
			container.normals = (GLfloat*)malloc(container.mesh_vertex_count * 3 * sizeof(GLfloat));
			for (int i = 0; i < container.mesh_vertex_count; i++)
			{
				const aiVector3D* vn = &(mesh->mNormals[i]);
				container.normals[i * 3] = (GLfloat)vn->x;
				container.normals[i * 3 + 1] = (GLfloat)vn->y;
				container.normals[i * 3 + 2] = (GLfloat)vn->z;
				container.initial_normals.push_back(container.normals[i * 3]);
				container.initial_normals.push_back(container.normals[i * 3 + 1]);
				container.initial_normals.push_back(container.normals[i * 3 + 2]);
			}
		}
		if (mesh->HasTextureCoords(0))
		{
			//cout << "There are texture vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;
			container.texcoords = (GLfloat*)malloc(container.mesh_vertex_count * 2 * sizeof(GLfloat));
			for (int i = 0; i < container.mesh_vertex_count; i++)
			{
				const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
				container.texcoords[i * 2] = (GLfloat)vt->x;
				container.texcoords[i * 2 + 1] = (GLfloat)vt->y;
				//cout << "[" << texcoords[i * 2] << "," << texcoords[i * 2 + 1] << "]" << endl;
			}
		}

		if (mesh->HasTangentsAndBitangents()) {
			//cout << "There are tangent vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;
			container.tangents = (GLfloat*)malloc(container.mesh_vertex_count * 4 * sizeof(GLfloat));
			for (int i = 0; i < container.mesh_vertex_count; i++)
			{
				const aiVector3D* tangent = &(mesh->mTangents[i]);
				const aiVector3D* bitangent = &(mesh->mBitangents[i]);
				const aiVector3D* normal = &(mesh->mNormals[i]);

				// put the three vectors into my vec3 struct format for doing maths
				vec3 t(tangent->x, tangent->y, tangent->z);
				vec3 n(normal->x, normal->y, normal->z);
				vec3 b(bitangent->x, bitangent->y, bitangent->z);
				// orthogonalise and normalise the tangent so we can use it in something
				// approximating a T,N,B inverse matrix
				vec3 t_i = normalise(t - n * dot(n, t));

				// get determinant of T,B,N 3x3 matrix by dot*cross method
				float det = (dot(cross(n, t), b));
				if (det < 0.0f) {
					det = -1.0f;
				}
				else {
					det = 1.0f;
				}

				// push back 4d vector for inverse tangent with determinant
				container.tangents[i * 4] = t_i.v[0];
				container.tangents[i * 4 + 1] = t_i.v[1];
				container.tangents[i * 4 + 2] = t_i.v[2];
				container.tangents[i * 4 + 3] = det;
			}
		}
		meshes.push_back(mesh);
	}
}

struct vertexWeight
{
	GLuint vertexID;
	float weight;
};

struct Bone
{
	string name;				//same name used for the corresponding Node
	vector<vertexWeight> weights;
	mat4 offsetWeight;
};

struct SkinnedMesh
{
	vector<GLfloat> initial_vertices = vector<GLfloat>();
	vector<GLfloat> initial_normals = vector<GLfloat>();
	GLfloat* vertices;
	GLfloat* normals;
	GLfloat* texcoords;
	GLfloat* tangents;
	int mesh_vertex_count;
	GLuint VAO;
};

struct Node	//Technically also a bone, but if it doesn't have a bone associated with it, then it doesn't influence vertices by itself
{
	mat4 transformation;
	vector<Node*> children;
	Node* parent;
	vector<SkinnedMesh> meshes;
	string name;				//same name used for the corresponding bone if there is one
};