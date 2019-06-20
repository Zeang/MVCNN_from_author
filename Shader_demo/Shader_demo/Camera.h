#pragma once

// Standard Includes
#include <vector>
#include <iostream>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;

enum Camera_Movement { U, D, L, R };	// Possible camera moving directions
const GLfloat STEP_KEY = 0.1f;			// Camera moving step
const GLfloat STEP_MOUSE = 0.01f;		// Camera moving step

// Default camera configuration
const glm::vec3 POSITION = glm::vec3(2.5f, -2.5f, 1.5f);
const glm::vec3 TARGET = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 UP = glm::vec3(0.0f, 0.0f, 1.0f);

class Camera
{
public:
	Camera(glm::vec3 p = POSITION, glm::vec3 t = TARGET, glm::vec3 u = UP);
	glm::mat4 getViewMatrix();
	glm::vec3 getPosition();

	void ProcessKeyboard(Camera_Movement direction);
	void ProcessMouseMovement(GLfloat x, GLfloat y);
	void ProcessMouseScroll(GLfloat yoffset);

	inline void assignPos(glm::vec3 p) { position = p;	updateDirections(); }
	inline void assignTar(glm::vec3 t) { target = t;	updateDirections(); }
	inline void assignUp(glm::vec3 u)  { up = u;		updateDirections(); }

private:
	glm::vec3 position, target, up;	// Essential vectors
	glm::vec3 front, right;			// Convinient vectors

	inline void updateDirections()
	{
		front = normalize(target - position);
		//up = UP;
		if (abs(glm::dot(front, up )) > 0.99)
			up = glm::vec3(0, 1, 0);
		right = normalize(glm::cross(front, up));
	}
};