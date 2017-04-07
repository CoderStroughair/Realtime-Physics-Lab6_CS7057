#include "Tetrahedral.h"

void print(Tetrahedral t)
{
	cout << "[" << *t.w[0] << "," << *t.w[1] << "," << *t.w[2] << "]\n";

	cout << "[" << *t.x[0] << "," << *t.x[1] << "," << *t.x[2] << "]\n";

	cout << "[" << *t.y[0] << "," << *t.y[1] << "," << *t.y[2] << "]\n";

	cout << "[" << *t.z[0] << "," << *t.z[1] << "," << *t.z[2] << "]\n";
}

void print(vector<Tetrahedral> t)
{
	for (int i = 0; i < t.size(); i++)
	{
		print(t[i]);
		cout << endl;
	}
}

TetrahedralisedMesh::TetrahedralisedMesh() {};

TetrahedralisedMesh::TetrahedralisedMesh(SingleMesh mesh, vec3 p) 
{
	position = p;
	base = mesh;
	CalculateTetrahedrals();
	print(tetrahedrals);
};

void TetrahedralisedMesh::CalculateTetrahedrals()
{
	tetrahedrals.clear();
	for (int i = 0; i < base.mesh_indice_count; i = i + 3)
	{
		Tetrahedral t;

		t.w.push_back(&position.v[0]);
		t.w.push_back(&position.v[1]);
		t.w.push_back(&position.v[2]);

		t.x.push_back(&base.newpoints[base.indices[i]*3]);
		t.x.push_back(&base.newpoints[base.indices[i]*3+1]);
		t.x.push_back(&base.newpoints[base.indices[i]*3+2]);

		t.y.push_back(&base.newpoints[base.indices[i] * 3 + 3]);
		t.y.push_back(&base.newpoints[base.indices[i] * 3 + 4]);
		t.y.push_back(&base.newpoints[base.indices[i] * 3 + 5]);

		t.z.push_back(&base.newpoints[base.indices[i] * 3 + 6]);
		t.z.push_back(&base.newpoints[base.indices[i] * 3 + 7]);
		t.z.push_back(&base.newpoints[base.indices[i] * 3 + 8]);

		tetrahedrals.push_back(t);
	}
}

void TetrahedralisedMesh::update()
{
	if(!drawTetra)
		CalculateTetrahedrals();
}

void TetrahedralisedMesh::BreakOffTetrahedrals()
{
	drawTetra = true;

	for (int i = 0; i < tetrahedrals.size(); i++)
	{
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].w[0]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].w[1]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].w[2]);

		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].x[0]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].x[1]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].x[2]);

		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].y[0]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].y[1]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].y[2]);

		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].z[0]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].z[1]);
		tetrahedrals[i].t.initialpoints.push_back(*tetrahedrals[i].z[2]);

		tetrahedrals[i].t.newpoints = tetrahedrals[i].t.initialpoints;

		tetrahedrals[i].t.mesh_vertex_count = 4;
		tetrahedrals[i].t.mesh_indice_count = 12;

		tetrahedrals[i].t.indices.push_back(0);
		tetrahedrals[i].t.indices.push_back(1);
		tetrahedrals[i].t.indices.push_back(2);

		tetrahedrals[i].t.indices.push_back(0);
		tetrahedrals[i].t.indices.push_back(2);
		tetrahedrals[i].t.indices.push_back(3);

		tetrahedrals[i].t.indices.push_back(1);
		tetrahedrals[i].t.indices.push_back(2);
		tetrahedrals[i].t.indices.push_back(3);

		tetrahedrals[i].t.indices.push_back(0);
		tetrahedrals[i].t.indices.push_back(1);
		tetrahedrals[i].t.indices.push_back(3);

		glGenVertexArrays(1, &tetrahedrals[i].t.VAO[0]);
		glBindVertexArray(tetrahedrals[i].t.VAO[0]);
		GLuint vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, tetrahedrals[i].t.mesh_vertex_count * 3 * sizeof(GLfloat), tetrahedrals[i].t.initialpoints.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tetrahedrals[i].t.mesh_indice_count * sizeof(GLuint), tetrahedrals[i].t.indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glEnableVertexAttribArray(0);
	}	
}

void TetrahedralisedMesh::Draw(mat4 model, mat4 view, mat4 proj, GLuint shaderID, EulerCamera cam)
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
	vec3 Kd = vec3(0.0f, 1.0f, 0.0f);
	vec3 Ka = vec3(0.0f, 1.0f, 0.0f)*0.2; // ambient reflectance
	float exp = 100.0f; //specular exponent - size of the specular elements

	mat4 newModel = translate(model, vec3(0, 0, 0));

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, view.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, proj.m);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, model.m);
	glUniform3fv(glGetUniformLocation(shaderID, "Ls"), 1, Ls.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Ld"), 1, Ld.v);
	glUniform3fv(glGetUniformLocation(shaderID, "La"), 1, La.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Ks"), 1, Ks.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Kd"), 1, Kd.v);
	glUniform3fv(glGetUniformLocation(shaderID, "Ka"), 1, Ka.v);
	glUniform1f(glGetUniformLocation(shaderID, "specular_exponent"), exp);
	glUniform3fv(glGetUniformLocation(shaderID, "light"), 1, light.v);
	glUniform4fv(glGetUniformLocation(shaderID, "camPos"), 1, cam.getPosition().v);

	if (!drawTetra)
	{
		glBindVertexArray(base.VAO[0]);
		if (!base.converted)
			glDrawArrays(GL_TRIANGLES, 0, base.mesh_vertex_count);
		else
			glDrawElements(GL_TRIANGLES, base.mesh_indice_count, GL_UNSIGNED_INT, 0);
	}
	else
	{
		for (int i = 0; i < tetrahedrals.size(); i++)
		{
			//newModel = translate(newModel, vec3(2, 2, 0));
			//glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, newModel.m);
			glBindVertexArray(tetrahedrals[i].t.VAO[0]);
			glDrawElements(GL_TRIANGLES, tetrahedrals[i].t.mesh_indice_count, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}
