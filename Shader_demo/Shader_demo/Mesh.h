#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TriMesh.h"
#include "Shader.h"

using namespace std;
using namespace trimesh;

struct Vertex 
{
	trimesh::vec3 Position;
	trimesh::vec3 Normal;
};

class Mesh 
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;

	Mesh();
	Mesh(vector<Vertex> vertices, vector<GLuint> indices);
	Mesh(string filename);
	void loadMesh(string filename, bool needFlat = true);
	void Draw(GLuint mode);
	glm::vec3 getCoordinates(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 viewport, glm::vec3 cameraPos, GLfloat coordX, GLfloat coordY);

private:
	GLuint VAO, VBO, EBO;
	void setupMesh();
	glm::vec3 getVector(trimesh::vec3 vec);
};

