#include "Viewport.h"

glm::vec3 Viewport::pointColor = glm::vec3(0.8f, 0.2f, 0.2f);
// fstream Viewport::file("pairs.txt", ios::out); // Not a good implementation, needs modifying

void Viewport::changePointColor()
{
	pointColor = glm::vec3((GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX, (GLfloat)rand() / RAND_MAX);
}

Viewport::Viewport(void)
{
}

Viewport::Viewport(glm::vec4 viewport, const char* modelName)
{
	init(viewport, modelName);
}

Viewport::~Viewport(void)
{
}

void Viewport::init(glm::vec4 viewport, const char* modelName)
{
	this->viewport = viewport;
	mesh.loadMesh(modelName);
	shader.loadShader("shaders/shader.vs", "shaders/shader.frag");
	pointsShader.loadShader("shaders/points.vs", "shaders/points.frag");
}

void Viewport::init(glm::vec4 viewport, Mesh mesh)
{
	this->viewport = viewport;
	this->mesh = mesh;
	shader.loadShader("shaders/shader.vs", "shaders/shader.frag");
	pointsShader.loadShader("shaders/points.vs", "shaders/points.frag");
}

void Viewport::updateFrame(GLuint polygenMode)
{
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

	// Activate shader
	shader.Use();

	// Set light color and position
	GLint lightColorLoc = glGetUniformLocation(shader.Program, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
	//glUniform3f(lightColorLoc, 0.9f, 0.9f, 0.9f);
	glm::vec3 lightPos = camera.getPosition();
	GLint lightPosLoc = glGetUniformLocation(shader.Program, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	// Create camera transformation
	model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	view = camera.getViewMatrix();
	projection = glm::perspective(45.0f, (GLfloat)viewport.z/viewport.w, 0.01f, 100.0f);
	//projection = glm::perspective(43.0f, 1.0f, 1.0f, 2.0f);
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	GLint viewLoc = glGetUniformLocation(shader.Program, "view");
	GLint projLoc = glGetUniformLocation(shader.Program, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draw
	glUniform3f(glGetUniformLocation(shader.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
	mesh.Draw(polygenMode);

	if(points.size() > 0)
	{
		pointsShader.Use();
		GLint modelLoc = glGetUniformLocation(pointsShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(pointsShader.Program, "view");
		GLint projLoc = glGetUniformLocation(pointsShader.Program, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		drawPoints();
	}
}

void Viewport::updateCamera(glm::vec3 targetPos, glm::vec3 cameraPos)
{
	camera.assignTar(targetPos);
	camera.assignPos(cameraPos);
}

void Viewport::updateUP(glm::vec3 up)
{
	camera.assignUp(up);
}

void Viewport::onMouseMove(GLfloat xoffset, GLfloat yoffset)
{
	camera.ProcessMouseMovement(xoffset, yoffset);
}

bool Viewport::onMouseClick(GLfloat x, GLfloat y)
{
	glm::vec3 coord = mesh.getCoordinates(projection, view, model, viewport, camera.getPosition(), x, y);
	if (coord != glm::vec3(0.0f, 0.0f, 0.0f))
	{
		Point point(coord, pointColor);
		points.push_back(point);
		//file << coord.x << ' ' << coord.y << ' ' << coord.z << endl;
		return true;
	}

	return false;
}

void Viewport::onScroll(GLfloat yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

bool Viewport::isInViewport(int x, int y)
{
	return (x > viewport.x && x < viewport.x + viewport.z \
		&& y > viewport.y && y < viewport.y + viewport.w);
}

void Viewport::drawPoints()
{
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), &points[0], GL_STATIC_DRAW);

	// Point Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)0);
	// Point Colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (GLvoid*)offsetof(Point, Color));

	glBindVertexArray(0);

	glPointSize(8.0f);
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, points.size());
	glBindVertexArray(0);
}