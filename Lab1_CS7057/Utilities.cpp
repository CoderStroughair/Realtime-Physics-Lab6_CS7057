#include "Utilities.h"
#include <iostream>
#include <string>
#include <vector>
#include "text.h"
#include <string>
#include <assert.h>
#include "Defines.h"
#include "Loader.h"


// Assimp includes
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

//uncomment if needed
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#pragma region FRAMEBUFFER



#pragma endregion

#pragma region MESH

Mesh::Mesh() {}

void Mesh::init(const char* mesh_file, const char* tex_file = NULL, const char* normal_file = NULL) {

	if (!load_mesh(mesh_file))
		exit(1);

	if (tex_file != NULL)
	{
		load_texture(tex_file, &tex);
		glUniform1i(tex, 0);
	}

	if (normal_file != NULL)
	{
		load_texture(normal_file, &norm);
		glUniform1i(norm, 1);
	}

}

void Mesh::initCubeMap(GLfloat vertices[], int num_vertices, string texture)
{
	this->VAO[0] = NULL;
	GLuint VBO;
	this->mesh_vertex_count = num_vertices;
	glGenVertexArrays(1, &VAO[0]);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * num_vertices, vertices, GL_STATIC_DRAW);

	for (int i = 0; i < num_vertices * 6; i += 6)
	{
		newpoints.push_back(vertices[i]);
		newpoints.push_back(vertices[i + 1]);
		newpoints.push_back(vertices[i + 2]);
	}

	glBindVertexArray(VAO[0]);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);	//Creating a Texture Buffer

	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X, (TEXTURE_FOLDER + texture + CUBEMAP_RIGHT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, (TEXTURE_FOLDER + texture + CUBEMAP_LEFT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, (TEXTURE_FOLDER + texture + CUBEMAP_TOP).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, (TEXTURE_FOLDER + texture + CUBEMAP_BOT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, (TEXTURE_FOLDER + texture + CUBEMAP_FRONT).c_str());
	loadCubeFace(tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, (TEXTURE_FOLDER + texture + CUBEMAP_BACK).c_str());

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//Changing the many options for textures to suit this situation
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Mesh::loadCubeFace(GLuint textureID, GLenum side, const char* filename)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int w;
	int h;
	int comp;
	unsigned char* image = stbi_load(filename, &w, &h, &comp, STBI_rgb);
	std::string name = filename;
	if (image == NULL)
	{
		cout << "Failed to load image" << endl;
		return;
	}
	//else
	//cout << name.substr(name.find_last_of('/') + 1, name.length()) << " [width, height]: [" << w << ", " << h << "]" << endl;
	if ((w & (w - 1)) != 0 || (h & (h - 1)) != 0)
	{
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", filename);
	}

	glTexImage2D(side, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
}

bool Mesh::load_mesh(const char* file_name)
{
	string name = file_name;
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (!scene)
	{
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
	}
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];

		GLfloat* points = NULL; // array of vertex points
		GLfloat* normals = NULL; // array of vertex normals
		GLfloat* texcoords = NULL; // array of texture coordinates
		GLfloat* tangents = NULL;
		//printf("    %i vertices in mesh[%i]\n", mesh->mNumVertices, i);

		/* pass back number of vertex points in mesh */
		mesh_vertex_count = mesh->mNumVertices;

		/* generate a VAO, using the pass-by-reference parameter that we give to the
		function */
		glGenVertexArrays(1, &VAO[i]);
		glBindVertexArray(VAO[i]);

		/* we really need to copy out all the data from AssImp's funny little data
		structures into pure contiguous arrays before we copy it into data buffers
		because assimp's texture coordinates are not really contiguous in memory.
		i allocate some dynamic memory to do this. */
		if (mesh->HasPositions())
		{
			points = (GLfloat*)malloc(mesh_vertex_count * 3 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* vp = &(mesh->mVertices[i]);
				points[i * 3] = (GLfloat)vp->x;
				points[i * 3 + 1] = (GLfloat)vp->y;
				points[i * 3 + 2] = (GLfloat)vp->z;
				newpoints.push_back(points[i * 3]);
				newpoints.push_back(points[i * 3 + 1]);
				newpoints.push_back(points[i * 3 + 2]);
			}
			initialpoints = newpoints;
		}
		if (mesh->HasNormals())
		{
			normals = (GLfloat*)malloc(mesh_vertex_count * 3 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* vn = &(mesh->mNormals[i]);
				normals[i * 3] = (GLfloat)vn->x;
				normals[i * 3 + 1] = (GLfloat)vn->y;
				normals[i * 3 + 2] = (GLfloat)vn->z;
				newnormals.push_back(normals[i * 3]);
				newnormals.push_back(normals[i * 3 + 1]);
				newnormals.push_back(normals[i * 3 + 2]);
			}
			initialnormals = newnormals;
		}
		if (mesh->HasTextureCoords(0))
		{
			//cout << "There are texture vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;
			texcoords = (GLfloat*)malloc(mesh_vertex_count * 2 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
			{
				const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
				texcoords[i * 2] = (GLfloat)vt->x;
				texcoords[i * 2 + 1] = (GLfloat)vt->y;
				//cout << "[" << texcoords[i * 2] << "," << texcoords[i * 2 + 1] << "]" << endl;
			}
		}

		if (mesh->HasTangentsAndBitangents()) {
			//cout << "There are tangent vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;
			tangents = (GLfloat*)malloc(mesh_vertex_count * 4 * sizeof(GLfloat));
			for (int i = 0; i < mesh_vertex_count; i++)
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
				tangents[i * 4] = t_i.v[0];
				tangents[i * 4 + 1] = t_i.v[1];
				tangents[i * 4 + 2] = t_i.v[2];
				tangents[i * 4 + 3] = det;
			}
		}
		//else
		//cout << "There are no tangent vertexes for mesh " << name.substr(name.find_last_of('/') + 1, name.length()) << endl;

		/* copy mesh data into VBOs */
		if (mesh->HasPositions())
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh_vertex_count * 3 * sizeof(GLfloat), points, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);
			free(points);
		}
		if (mesh->HasNormals())
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 3 * mesh_vertex_count * sizeof(GLfloat), normals, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);
			free(normals);
		}
		if (mesh->HasTextureCoords(0))
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 2 * mesh_vertex_count * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(2);
			free(texcoords);
		}
		if (mesh->HasTangentsAndBitangents())
		{
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, 4 * mesh_vertex_count * sizeof(GLfloat), tangents, GL_STATIC_DRAW);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(3);
			free(tangents);
		}

	}
	aiReleaseImport(scene);
	return true;
}

bool Mesh::update_mesh(mat4 orientation, vec3 position)
{
	for (int i = 0; i < mesh_vertex_count; i++)
	{
		vec3 vertice = vec3(initialpoints[i * 3], initialpoints[i * 3 + 1], initialpoints[i * 3 + 2]);
		vertice = multiply(orientation, vertice) + position;
		newpoints[i * 3] = vertice.v[0];
		newpoints[i * 3 + 1] = vertice.v[1];
		newpoints[i * 3 + 2] = vertice.v[2];

		vertice = vec3(initialnormals[i * 3], initialnormals[i * 3 + 1], initialnormals[i * 3 + 2]);
		vertice = multiply(orientation, vertice) + position;
		newnormals[i * 3] = vertice.v[0];
		newnormals[i * 3 + 1] = vertice.v[1];
		newnormals[i * 3 + 2] = vertice.v[2];
	}



	glBindVertexArray(VAO[0]);
	/* copy mesh data into VBOs */
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_vertex_count * 3 * sizeof(GLfloat), newpoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint vbo2;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 3 * mesh_vertex_count * sizeof(GLfloat), newnormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	return true;
}

bool Mesh::scale_mesh(GLfloat scale)
{
	for (int i = 0; i < mesh_vertex_count; i++)
	{
		vec3 vertice = vec3(initialpoints[i * 3], initialpoints[i * 3 + 1], initialpoints[i * 3 + 2]);
		vertice *= scale;
		newpoints[i * 3] = vertice.v[0];
		newpoints[i * 3 + 1] = vertice.v[1];
		newpoints[i * 3 + 2] = vertice.v[2];

		vertice = vec3(initialnormals[i * 3], initialnormals[i * 3 + 1], initialnormals[i * 3 + 2]);
		vertice *= scale;
		vertice = normalise(vertice);
		newnormals[i * 3] = vertice.v[0];
		newnormals[i * 3 + 1] = vertice.v[1];
		newnormals[i * 3 + 2] = vertice.v[2];
	}



	glBindVertexArray(VAO[0]);
	/* copy mesh data into VBOs */
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_vertex_count * 3 * sizeof(GLfloat), newpoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint vbo2;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 3 * mesh_vertex_count * sizeof(GLfloat), newnormals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	return true;
}

bool Mesh::load_texture(const char* file_name, GLuint* tex)
{
	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
	if (!image_data)
	{
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0)
	{
		fprintf(stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name);
	}
	int width_in_bytes = x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++)
	{
		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures(1, tex);
	//glActiveTexture(mode);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	return true;
}

#pragma endregion

#pragma region CAMERA

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

inline void EulerCamera::changeFront(GLfloat pi, GLfloat ya, GLfloat ro) {

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

inline void EulerCamera::changeFront()
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

#pragma endregion

#pragma region SHADER

// Create a NULL-terminated string by reading the provided file
char* Shader::readShaderSource(const char* shaderFile) {

	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}

void Shader::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint Shader::CompileShader(char* vertex, char* fragment) {

	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	if (vertex)
		AddShader(shaderProgramID, vertex, GL_VERTEX_SHADER);
	else
	{
		cout << "No Vertex Shader found" << endl;
		AddShader(shaderProgramID, DEFAULT_VERT, GL_VERTEX_SHADER);
	}
	if (fragment)
		AddShader(shaderProgramID, fragment, GL_FRAGMENT_SHADER);
	else
	{
		cout << "No Fragment Shader found" << endl;
		AddShader(shaderProgramID, DEFAULT_FRAG, GL_FRAGMENT_SHADER);
	}

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;

}

#pragma endregion

/*----------------------------------------------------------------------------
							IMPLEMENTATIONS
----------------------------------------------------------------------------*/

void drawObject(GLuint shaderID, mat4 view, mat4 proj, mat4 model, vec3 light, vec3 Ls, vec3 La, vec3 Ld, vec3 Ks, vec3 Ka, vec3 Kd, float exp, EulerCamera cam, Mesh mesh, float coneAngle, vec3 coneDirection, GLenum mode)
{

	//for (int i = 0; i < 6; i++)
	//{
	glUseProgram(shaderID);
	glBindVertexArray(mesh.VAO[0]);
	glUniform3fv(glGetUniformLocation(shaderID, "Ls"), 1, Ls.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Ld"), 1, Ld.v);
	glUniform3fv(glGetUniformLocation(shaderID, "La"), 1, La.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Ks"), 1, Ks.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Kd"), 1, Kd.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Ka"), 1, Ka.v);
	glUniform1f(glGetUniformLocation(shaderID, "specular_exponent"), exp);
	glUniform3fv(glGetUniformLocation(shaderID, "light"), 1, light.v);
	glUniform4fv(glGetUniformLocation(shaderID, "camPos"), 1, cam.getPosition().v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, view.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, proj.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);

	if (coneAngle != NULL)
	{
		glUniform3fv(glGetUniformLocation(shaderID, "coneDirection"), 1, coneDirection.v);
		glUniform1f(glGetUniformLocation(shaderID, "coneAngle"), coneAngle);
	}

	if (mesh.tex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.tex);
		glUniform1i(glGetUniformLocation(shaderID, "texture_primary"), 0);

	}
	if (mesh.norm)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh.norm);
		glUniform1i(glGetUniformLocation(shaderID, "normal_map"), 1);

	}

	glDrawArrays(mode, 0, mesh.mesh_vertex_count);
	//}
}

void drawObjectDebug(GLuint shaderID, mat4 view, mat4 proj, mat4 model, vec3 light, vec3 Ls, vec3 La, vec3 Ld, vec3 Ks, vec3 Ka, vec3 Kd, float exp, EulerCamera cam, Mesh mesh, float coneAngle, vec3 coneDirection, GLenum mode)
{

	//for (int i = 0; i < 6; i++)
	//{
	glUseProgram(shaderID);
	glBindVertexArray(mesh.VAO[0]);

	GLuint temp = glGetUniformLocation(shaderID, "Ls");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ls.v);
	temp = glGetUniformLocation(shaderID, "Ld");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ld.v);
	temp = glGetUniformLocation(shaderID, "La");
	cout << temp << endl;
	glUniform3fv(temp, 1, La.v);
	temp = glGetUniformLocation(shaderID, "Ks");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ks.v);
	temp = glGetUniformLocation(shaderID, "Kd");
	cout << temp << endl;
	glUniform3fv(temp, 1, Kd.v);
	temp = glGetUniformLocation(shaderID, "Ka");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ka.v);
	temp = glGetUniformLocation(shaderID, "specular_exponent");
	cout << temp << endl;
	glUniform1f(temp, exp);
	temp = glGetUniformLocation(shaderID, "light");
	cout << temp << endl;
	glUniform3fv(temp, 1, light.v);
	glUniform4fv(glGetUniformLocation(shaderID, "camPos"), 1, cam.getPosition().v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, view.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, proj.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);

	if (coneAngle != NULL)
	{
		glUniform3fv(glGetUniformLocation(shaderID, "coneDirection"), 1, coneDirection.v);
		glUniform1f(glGetUniformLocation(shaderID, "coneAngle"), coneAngle);
	}

	if (mesh.tex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh.tex);
		glUniform1i(glGetUniformLocation(shaderID, "texture_primary"), 0);

	}
	if (mesh.norm)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh.norm);
		glUniform1i(glGetUniformLocation(shaderID, "normal_map"), 1);

	}

	glDrawArrays(mode, 0, mesh.mesh_vertex_count);
	//}
}

void drawCubeMap(GLuint shaderID, GLuint textureID, mat4 view, mat4 proj, mat4 model, vec3 Ld, vec3 La, EulerCamera cam, Mesh skybox, GLenum mode)
{
	glDepthMask(GL_FALSE);
	glUseProgram(shaderID);
	glBindVertexArray(skybox.VAO[0]);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderID, "cube_texture"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, proj.m);
	view.m[12] = 0;
	view.m[13] = 0;
	view.m[14] = 0;
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, view.m);
	glDrawArrays(mode, 0, skybox.mesh_vertex_count);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}

void drawLine(GLuint shaderID, mat4 model, mat4 proj, vec3 origin, vec3 destination, vec3 colour)
{
	GLfloat points[] =
	{
		origin.v[0], origin.v[1], origin.v[2],
		destination.v[0], destination.v[1], destination.v[2]
	};
	GLuint vao, vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderID);
	glBindVertexArray(vao);
	glUniform3fv(glGetUniformLocation(shaderID, "colour"), 1, colour.v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);
	glDrawArrays(GL_LINES, 0, 2);
}

void drawTriangle(GLuint shaderID, mat4 model, mat4 proj, vec3 v1, vec3 v2, vec3 v3, vec3 colour)
{
	GLfloat points[] =
	{
		v1.v[0], v1.v[1], v1.v[2],
		v2.v[0], v2.v[1], v2.v[2],
		v3.v[0], v3.v[1], v3.v[2]
	};
	GLuint vao, vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderID);
	glBindVertexArray(vao);
	glUniform3fv(glGetUniformLocation(shaderID, "colour"), 1, colour.v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawPoint(GLuint shaderID, mat4 model, mat4 proj, vec3 point, vec3 colour)
{
	GLfloat p[] =
	{
		point.v[0], point.v[1], point.v[2]
	};
	GLuint vao, vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), p, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glUseProgram(shaderID);
	glBindVertexArray(vao);
	glUniform3fv(glGetUniformLocation(shaderID, "colour"), 1, colour.v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);
	glDrawArrays(GL_POINTS, 0, 1);
}

void drawModel(GLuint shaderID, mat4 view, mat4 model, mat4 proj, Model meshes, EulerCamera cam)
{
	glUseProgram(shaderID);

	vec3 Ls = vec3(0.6f, 0.3f, 0.6f);	//Specular Reflected Light
	vec3 Ld = vec3(0.8f, 0.8f, 0.8f);	//Diffuse Surface Reflectance
	vec3 La = vec3(0.8f, 0.8f, 0.8f);	//Ambient Reflected Light
	vec3 light = vec3(1.8*cos(0.0f), 1.8*sin(0.0f) + 1.0f, -5.0f);//light source location
	vec3 coneDirection = light + vec3(0.0f, -1.0f, 0.0f);
	float coneAngle = 40.0f;
	// object colour
	vec3 Ks = vec3(0.01f, 0.01f, 0.01f); // specular reflectance
	vec3 Kd = PURPLE;
	vec3 Ka = BLUE*0.2; // ambient reflectance
	float specular_exponent = 100.0f; //specular exponent - size of the specular elements

	GLuint temp = glGetUniformLocation(shaderID, "Ls");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ls.v);
	temp = glGetUniformLocation(shaderID, "Ld");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ld.v);
	temp = glGetUniformLocation(shaderID, "La");
	cout << temp << endl;
	glUniform3fv(temp, 1, La.v);
	temp = glGetUniformLocation(shaderID, "Ks");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ks.v);
	temp = glGetUniformLocation(shaderID, "Kd");
	cout << temp << endl;
	glUniform3fv(temp, 1, Kd.v);
	temp = glGetUniformLocation(shaderID, "Ka");
	cout << temp << endl;
	glUniform3fv(temp, 1, Ka.v);
	temp = glGetUniformLocation(shaderID, "specular_exponent");
	cout << temp << endl;
	glUniform1f(temp, specular_exponent);
	temp = glGetUniformLocation(shaderID, "light");
	cout << temp << endl;
	glUniform3fv(temp, 1, light.v);
	glUniform4fv(glGetUniformLocation(shaderID, "camPos"), 1, cam.getPosition().v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, view.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, proj.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);

	meshes.Draw(shaderID);
}