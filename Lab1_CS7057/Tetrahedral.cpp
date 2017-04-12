#include "Tetrahedral.h"

void print(Tetrahedral t)
{
	cout << "[" << t.w[0] << "," << t.w[1] << "," << t.w[2] << "]\n";

	cout << "[" << t.x[0] << "," << t.x[1] << "," << t.x[2] << "]\n";
	
	cout << "[" << t.y[0] << "," << t.y[1] << "," << t.y[2] << "]\n";

	cout << "[" << t.z[0] << "," << t.z[1] << "," << t.z[2] << "]\n";
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
	centerNode = p;
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
		t.w.push_back(0);
		t.w.push_back(0);
		t.w.push_back(0);

		t.x.push_back(base.initialpoints[base.indices[i] * 3]);
		t.x.push_back(base.initialpoints[base.indices[i] * 3 + 1]);
		t.x.push_back(base.initialpoints[base.indices[i] * 3 + 2]);

		t.y.push_back(base.initialpoints[base.indices[i+1] * 3]);
		t.y.push_back(base.initialpoints[base.indices[i+1] * 3 + 1]);
		t.y.push_back(base.initialpoints[base.indices[i+1] * 3 + 2]);

		t.z.push_back(base.initialpoints[base.indices[i+2] * 3]);
		t.z.push_back(base.initialpoints[base.indices[i+2] * 3 + 1]);
		t.z.push_back(base.initialpoints[base.indices[i+2] * 3 + 2]);

		vec3 w = vec3(0, 0, 0);
		vec3 x = vec3(base.initialpoints[base.indices[i] * 3], base.initialpoints[base.indices[i] * 3 + 1], base.initialpoints[base.indices[i] * 3 + 2]);
		vec3 y = vec3(base.initialpoints[base.indices[i + 1] * 3], base.initialpoints[base.indices[i + 1] * 3 + 1], base.initialpoints[base.indices[i + 1] * 3 + 2]);
		vec3 z = vec3(base.initialpoints[base.indices[i + 2] * 3], base.initialpoints[base.indices[i + 2] * 3 + 1], base.initialpoints[base.indices[i + 2] * 3 + 2]);

		t.faceA[0] = w;
		t.faceA[1] = x;
		t.faceA[2] = y;

		t.faceA[0] = x;
		t.faceA[1] = y;
		t.faceA[2] = z;

		t.faceA[0] = y;
		t.faceA[1] = z;
		t.faceA[2] = w;

		t.faceA[0] = z;
		t.faceA[1] = w;
		t.faceA[2] = x;

		//print(t);
		t.m = glm::mat4(
			t.w[0], t.w[1], t.w[2], 1.0,
			t.x[0], t.x[1], t.x[2], 1.0,
			t.y[0], t.y[1], t.y[2], 1.0,
			t.z[0], t.z[1], t.z[2], 1.0);
		t.mInv = inverse(t.m);
		mat4 temp = mat4
		(	t.mInv[0][0], t.mInv[0][1], t.mInv[0][2], t.mInv[0][3],
			t.mInv[1][0], t.mInv[1][1], t.mInv[0][2], t.mInv[0][3],
			t.mInv[2][0], t.mInv[2][1], t.mInv[0][2], t.mInv[0][3],
			t.mInv[3][0], t.mInv[3][1], t.mInv[0][2], t.mInv[0][3]);
		temp = qrDecomposition(temp);
		t.mInv = glm::mat4
		(	temp.m[0], temp.m[1], temp.m[2], temp.m[3],
			temp.m[4], temp.m[5], temp.m[6], temp.m[7],
			temp.m[8], temp.m[9], temp.m[10], temp.m[11],
			temp.m[12], temp.m[13], temp.m[14], temp.m[15]);
		tetrahedrals.push_back(t);
	}
}

void TetrahedralisedMesh::update()
{
	
}

void TetrahedralisedMesh::BreakOffTetrahedrals()
{
	drawTetra = true;

	for (int i = 0; i < tetrahedrals.size(); i++)
	{
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].w[0]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].w[1]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].w[2]);

		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].x[0]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].x[1]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].x[2]);

		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].y[0]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].y[1]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].y[2]);

		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].z[0]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].z[1]);
		tetrahedrals[i].t.initialpoints.push_back(tetrahedrals[i].z[2]);

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
			//newModel = translate(newModel, vec3(1, 0, 0));
			//glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, newModel.m);
			glBindVertexArray(tetrahedrals[i].t.VAO[0]);
			glDrawElements(GL_TRIANGLES, tetrahedrals[i].t.mesh_indice_count, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void TetrahedralisedMesh::ApplyForce(vec3 point, vec3 force)
{
	for (int i = 0; i < tetrahedrals.size(); i++)
	{
		if (tetrahedrals[i].affected(point, collisionRadius))
		{
			cout << "I'm affected" << endl;
			float e = 0.001;	//accuracy that we want in our Newton-Raphson Method
			//vector<glm::vec3> 
		}
	}
}

//Adapted from http://stackoverflow.com/questions/38545520/barycentric-coordinates-of-a-tetrahedron
vec4 TetrahedralisedMesh::GetBarycentricCoords(vec3 a, vec3 b, vec3 c, vec3 d, vec3 p)
{
	vec3 vap = p - a;
	vec3 vbp = p - b;

	vec3 vab = b - a;
	vec3 vac = c - a;
	vec3 vad = d - a;

	vec3 vbc = c - b;
	vec3 vbd = d - b;
	// ScTP computes the scalar triple product
	float va6 = scalarTripleProduct(vbp, vbd, vbc);
	float vb6 = scalarTripleProduct(vap, vac, vad);
	float vc6 = scalarTripleProduct(vap, vad, vab);
	float vd6 = scalarTripleProduct(vap, vab, vac);
	float v6 = 1 / scalarTripleProduct(vab, vac, vad);
	return vec4(va6*v6, vb6*v6, vc6*v6, vd6*v6);
}


vec3 getClosestPointTriangle(vec3 triangle[3], vec3 p0)
{
	vec3 p1 = triangle[0];
	vec3 p2 = triangle[1];
	vec3 p3 = triangle[2];

	//Check Each Voronoi Region

	//Vertex Regions:

	//p1
	if (dot(p0 - p1, p3 - p1) <= 0 && dot(p0 - p1, p2 - p1) <= 0)
	{
		return p1;
	}
	//p2
	if (dot(p0 - p2, p3 - p2) <= 0 && dot(p0 - p2, p1 - p2) <= 0)
	{
		return p2;
	}
	//p3
	if (dot(p0 - p3, p2 - p3) <= 0 && dot(p0 - p3, p1 - p3) <= 0)
	{
		return p3;
	}

	//Edge Regions:

	//<p1, p2>
	if (dot(cross(cross(p3 - p2, p1 - p2), p1 - p2), p0 - p2) >= 0 && dot(p0 - p1, p2 - p1) >= 0 && dot(p0 - p2, p1 - p2) >= 0)
	{
		vec3 line[] = { p1, p2 };
		return getClosestPointLine(line, p0);
	}
	// <p2, p3>
	if (dot(cross(cross(p1 - p3, p2 - p3), p2 - p3), p0 - p3) >= 0 && dot(p0 - p2, p3 - p2) >= 0 && dot(p0 - p3, p2 - p3) >= 0)
	{
		vec3 line[] = { p2, p3 };
		return getClosestPointLine(line, p0);
	}
	// <p3, p1>
	if (dot(cross(cross(p2 - p1, p3 - p1), p3 - p1), p0 - p1) >= 0 && dot(p0 - p3, p1 - p3) >= 0 && dot(p0 - p1, p3 - p1) >= 0)
	{
		vec3 line[] = { p3, p1 };
		return getClosestPointLine(line, p0);
	}

	//It must be the Face Region
	return getClosestPointPlane(cross(p2, p1), p1, p0);
}
vec3 getClosestPointLine(vec3 line[], vec3 p0)
{
	vec3 u = line[1] - line[0];
	return line[0] + normalise(u)*(dot(p0 - line[0], normalise(u)));
}
vec3 getClosestPointPlane(vec3 normal, vec3 p1, vec3 p0)
{
	normal = normalise(normal);
	return (p0 - normal*dot((p0 - p1), normal));
}
float getDistance(vec3 v0, vec3 v1)
{
	float x_sq = (v0.v[0] - v1.v[0]) * (v0.v[0] - v1.v[0]);
	float y_sq = (v0.v[1] - v1.v[1]) * (v0.v[1] - v1.v[1]);
	float z_sq = (v0.v[2] - v1.v[2]) * (v0.v[2] - v1.v[2]);
	return sqrt(x_sq + y_sq + z_sq);
}