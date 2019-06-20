#include "Camera.h"

Camera::Camera(glm::vec3 p, glm::vec3 t, glm::vec3 u)
{
	position = p;
	target = t;
	up = u;
	updateDirections();
}

glm::mat4 Camera::getViewMatrix()
{
	//std::cout << position[0] << " " << position[1] << " "  << position[2] << std::endl;
	//std::cout << target[0] << " " << target[1] << " "  << target[2] << std::endl;
	//std::cout << up[0] << " " << up[1] << " "  << up[2] << std::endl;
	return lookAt(position, target, up);
}

glm::vec3 Camera::getPosition()
{
	return position;
}

void Camera::ProcessKeyboard(Camera_Movement direction)
{
	switch(direction)
	{
	case U:
		position = rotate(position, -STEP_KEY, right);
		break;
	case D:
		position = rotate(position, STEP_KEY, right);
		break;
	case L:
		position = rotate(position, -STEP_KEY, up);
		break;
	case R:
		position = rotate(position, STEP_KEY, up);
		break;
	}
	updateDirections();
}

void Camera::ProcessMouseMovement(GLfloat x, GLfloat y)
{
	position = rotate(position, - x * STEP_MOUSE, up);
	position = rotate(position, - y * STEP_MOUSE, right);

	updateDirections();
}

void Camera::ProcessMouseScroll(GLfloat yoffset)
{
	position += yoffset * front * STEP_KEY;
}
