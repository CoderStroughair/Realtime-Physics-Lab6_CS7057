#pragma once 
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include "Antons_maths_funcs.h"
#include <GL/freeglut.h>
#include "text.h"
#include <string>

#include "Defines.h"
#include "Model.h"
#include "EulerCamera.h"
#include "SingleMeshLoader.h"
#include "Framebuffer.h"
#include "Shader.h"
#include "Tetrahedral.h"

/*----------------------------------------------------------------------------
								DEFINITIONS
----------------------------------------------------------------------------*/

void drawObject(GLuint shaderID, mat4 view, mat4 proj, mat4 model, vec3 light, vec3 Ls, vec3 La, vec3 Ld, vec3 Ks, vec3 Ka, vec3 Kd, float exp, EulerCamera cam, SingleMesh mesh, float coneAngle, vec3 coneDirection, GLenum mode);
void drawCubeMap(GLuint shaderID, GLuint textureID, mat4 view, mat4 proj, mat4 model, vec3 Ld, vec3 La, EulerCamera cam, SingleMesh skybox, GLenum mode);
void drawLine(GLuint shaderID, mat4 model, mat4 proj, vec3 origin, vec3 destination, vec3 colour);
void drawTriangle(GLuint shaderID, mat4 model, mat4 proj, vec3 v1, vec3 v2, vec3 v3, vec3 colour);
void drawPoint(GLuint shaderID, mat4 model, mat4 proj, vec3 point, vec3 colour);
void drawModel(GLuint shaderID, mat4 view, mat4 model, mat4 proj, Model meshes, EulerCamera cam);

/*----------------------------------------------------------------------------
								IMPLEMENTATIONS
----------------------------------------------------------------------------*/

void drawObject(GLuint shaderID, mat4 view, mat4 proj, mat4 model, vec3 light, vec3 Ls, vec3 La, vec3 Ld, vec3 Ks, vec3 Ka, vec3 Kd, float exp, EulerCamera cam, SingleMesh mesh, float coneAngle, vec3 coneDirection, GLenum mode)
{

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

	if (!mesh.converted)
		glDrawArrays(mode, 0, mesh.mesh_vertex_count);
	else
		glDrawElements(GL_TRIANGLES, mesh.mesh_indice_count, GL_UNSIGNED_INT, 0);

}

void drawObjectDebug(GLuint shaderID, mat4 view, mat4 proj, mat4 model, vec3 light, vec3 Ls, vec3 La, vec3 Ld, vec3 Ks, vec3 Ka, vec3 Kd, float exp, EulerCamera cam, SingleMesh mesh, float coneAngle, vec3 coneDirection, GLenum mode)
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

void drawCubeMap(GLuint shaderID, GLuint textureID, mat4 view, mat4 proj, mat4 model, vec3 Ld, vec3 La, EulerCamera cam, SingleMesh skybox, GLenum mode)
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
	glUniform3fv(temp, 1, Ls.v);
	temp = glGetUniformLocation(shaderID, "Ld");
	glUniform3fv(temp, 1, Ld.v);
	temp = glGetUniformLocation(shaderID, "La");
	glUniform3fv(temp, 1, La.v);
	temp = glGetUniformLocation(shaderID, "Ks");
	glUniform3fv(temp, 1, Ks.v);
	temp = glGetUniformLocation(shaderID, "Kd");
	glUniform3fv(temp, 1, Kd.v);
	temp = glGetUniformLocation(shaderID, "Ka");
	glUniform3fv(temp, 1, Ka.v);
	temp = glGetUniformLocation(shaderID, "specular_exponent");
	glUniform1f(temp, specular_exponent);
	temp = glGetUniformLocation(shaderID, "light");
	glUniform3fv(temp, 1, light.v);
	glUniform4fv(glGetUniformLocation(shaderID, "camPos"), 1, cam.getPosition().v);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, view.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, proj.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);

	meshes.Draw(shaderID);
}
