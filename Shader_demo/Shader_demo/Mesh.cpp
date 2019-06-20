#include "Mesh.h"

Mesh::Mesh(){}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices) 
{
	this->vertices = vertices;
	this->indices = indices;
	this->setupMesh();
}

Mesh::Mesh(string filename)
{
	loadMesh(filename);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	glBindVertexArray(0);
}

void Mesh::loadMesh(string filename, bool needFlat)
{
	TriMesh *mesh;
	mesh = TriMesh::read(filename);
	mesh->normals.clear();
	mesh->need_normals();
	mesh->need_faces();

	if (needFlat)
	{
		vertices.resize(mesh->faces.size()*3);
		indices.resize(mesh->faces.size()*3);
		for (size_t i = 0 ; i < mesh->faces.size(); i++)
		{
			vec3 fnormal = mesh->trinorm(i);
			fnormal = normalize(fnormal);
			Vertex v0;
			v0.Position = mesh->vertices[mesh->faces[i][0]];
			v0.Normal = fnormal;
			vertices[i * 3] = v0;
			Vertex v1;
			v1.Position = mesh->vertices[mesh->faces[i][1]];
			v1.Normal = fnormal;
			vertices[i * 3 + 1] = v1;
			Vertex v2;
			v2.Position = mesh->vertices[mesh->faces[i][2]];
			v2.Normal = fnormal;
			vertices[i * 3 + 2] = v2;
			indices[i * 3] = i * 3;
			indices[i * 3 + 1] = i * 3 + 1;
			indices[i * 3 + 2] = i * 3 + 2;
		}
	}
	else // default as smooth
	{
		for(size_t i = 0; i < mesh->vertices.size(); i++)
		{
			Vertex v;
			v.Position = mesh->vertices[i];
			v.Normal = mesh->normals[i];
			vertices.push_back(v);
		}
		for(size_t i = 0 ; i < mesh->faces.size(); i++)
		{
			indices.push_back(mesh->faces[i][0]);
			indices.push_back(mesh->faces[i][1]);
			indices.push_back(mesh->faces[i][2]);
		}
	}

	this->setupMesh();
}

void Mesh::Draw(GLuint polygonMode)
{
	// Draw mesh
	glBindVertexArray(this->VAO);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::vec3 Mesh::getCoordinates(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 viewport, glm::vec3 cameraPos, GLfloat coordX, GLfloat coordY)
{
	vector<glm::vec3> points;
	glm::vec2 pt(coordX, coordY);

	// Search for the triangles that the click point in
	for (size_t i = 0; i < vertices.size() - 3; i += 3)
	{
		glm::vec3 v1 = getVector(vertices[i].Position), v2 = getVector(vertices[i + 1].Position), v3 = getVector(vertices[i + 2].Position);

		glm::vec4 c = projection * view * model * glm::vec4(v1, 1.0f);
		GLfloat x = viewport.x + viewport.z / 2 * (c.x / c.w + 1), y = viewport.y + viewport.w - viewport.w / 2 * (c.y / c.w + 1);	// coordinate on the screen
		glm::vec2 p1(x, y);
		c = projection * view * model * glm::vec4(v2, 1.0f);
		x = viewport.x + viewport.z / 2 * (c.x / c.w + 1), y = viewport.y + viewport.w - viewport.w / 2 * (c.y / c.w + 1);
		glm::vec2 p2(x, y);
		c = projection * view * model * glm::vec4(v3, 1.0f);
		x = viewport.x + viewport.z / 2 * (c.x / c.w + 1), y = viewport.y + viewport.w - viewport.w / 2 * (c.y / c.w + 1);
		glm::vec2 p3(x, y);

		if( p1 != p2 && p1 != p3)
		{
			glm::vec2 lambda;
			lambda.x = ((p2.y - p3.y) * (pt.x - p3.x) + (p3.x - p2.x) * (pt.y - p3.y)) / ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));
			lambda.y = ((p3.y - p1.y) * (pt.x - p3.x) + (p1.x - p3.x) * (pt.y - p3.y)) / ((p2.y - p3.y) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.y - p3.y));


			// Save the position of the click point in the original space
			if (0 < lambda.x && lambda.x < 1 && 0 < lambda.y && lambda.y < 1 && 0 < 1 - lambda.x - lambda.y && 1 - lambda.x - lambda.y < 1)
				points.push_back(glm::vec3(v1 * lambda.x + v2 * lambda.y + v3 * (1 - lambda.x - lambda.y))); // for test
		}
	}

	// Find the point closest to the camera
	if (points.size() > 0)
	{
		GLfloat distance = glm::distance(points[0], cameraPos);
		glm::vec3 select = points[0];
		for (size_t i = 1; i < points.size(); i++)
		{
			if (distance > glm::distance(points[i], cameraPos))
			{
				distance = glm::distance(points[i], cameraPos);
				select = points[i];
			}
		}

		return select + 0.001f * cameraPos;
	}

	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 Mesh::getVector(trimesh::vec3 vec)
{
	return glm::vec3(vec[0], vec[1], vec[2]);
}

