#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLfloat
	winWidth = 800, winHeight = 1000;

GLuint
	VaoId,
	VboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation;

glm::mat4
	myMatrix, resizeMatrix;

float xMin = -400, xMax = 400, yMin = -500, yMax = 500;

void CreateVBO(void)
{
	GLfloat Vertices[] = {
		-50.0f,  100.0f, 0.0f, 1.0f,
		-50.0f,  -100.0f, 0.0f, 1.0f,
		50.0f,  -100.0f, 0.0f, 1.0f,
		50.0f,  100.0f, 0.0f, 1.0f
	};

	GLfloat Colors[] = {
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f,
	  1.0f, 0.5f, 0.2f, 1.0f
	};

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("example.vert", "example.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(0.25f, 0.15f, 0.0f, 1.0f);
	CreateVBO();
	CreateShaders();

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawArrays(GL_POLYGON, 0, 4);

	glFlush();
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	glutInitWindowPosition(screenWidth / 2 - (winWidth / 2), 0);
	glutInitWindowSize(winWidth, winHeight);

	glutCreateWindow("Car Passing Simulator");

	glewInit();
	Initialize();

	glutDisplayFunc(RenderFunction);
	glutCloseFunc(Cleanup);

	glutMainLoop();
}

