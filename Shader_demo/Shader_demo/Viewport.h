#pragma once

// Standard includes
#include <iostream>
#include <fstream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

// OpenCV
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace glm;

struct Point 
{
	Point(glm::vec3 pos, glm::vec3 color)
	{
		Position = pos;
		Color = color;
	}
	glm::vec3 Position;
	glm::vec3 Color;
};

class Viewport
{
public:
	Viewport(void);
	Viewport(glm::vec4 viewport, const char* modelName);
	~Viewport(void);

	void init(glm::vec4 viewport, const char* modelName);
	void init(glm::vec4 viewport, Mesh mesh);
	void updateFrame(GLuint polygenMode);
	void updateCamera(glm::vec3 targetPos, glm::vec3 cameraPos);
	void updateUP(glm::vec3 up);

	void onMouseMove(GLfloat xoffset, GLfloat yoffset);
	bool onMouseClick(GLfloat x, GLfloat y);
	void onScroll(GLfloat yoffset);

	bool isInViewport(int x, int y);

	static glm::vec3 pointColor;
	static void changePointColor();
	//static fstream file;

private:
	glm::vec4 viewport;
	glm::vec3 lightPos;
	glm::mat4 model, view, projection;

	Camera camera;
	Mesh mesh;
	Shader shader, pointsShader;

	vector<Point> points;
	void drawPoints();
};
