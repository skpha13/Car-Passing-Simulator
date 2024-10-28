#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "loadShaders.h"
#include "SOIL.h"


GLfloat
	winWidth = 800, winHeight = 1000;

GLuint
	VaoId,
	VboId,
	EboId,
	ProgramId,
	myMatrixLocation;

GLuint
	texture_car_01,
	texture_car_02,
	texture_tree,
	texture_asphalt,
	texture_curbs;

glm::mat4
	myMatrix, resizeMatrix;

float xMin = -400, xMax = 400, yMin = -500, yMax = 500;

void LoadTexture(const char* texturePath, GLuint& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CreateVBO(void)
{
	GLfloat Vertices[] = {
		// car vertices
		-50.0f,   100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		-50.0f,  -100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 50.0f,  -100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 50.0f,   100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 1.0f,

		// road vertices
		-175.0f,   500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		-175.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 175.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 175.0f,   500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 1.0f,

		// curbs vertices
			// left
		-200.0f,   500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f,
		-200.0f,  -500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f,
		-175.0f,  -500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f,
		-175.0f,   500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   1.0f, 1.0f,

		// tree
		-375.0f,  -250.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		-375.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
		-225.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-225.0f,  -250.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	};

	GLuint Indices[] = {
		 // car_01 indices
		 0,  1,  2,  3,
		 // car_02 indices
		 4,  5,  6,  7,
		 
		 // curb indices
		 8,  9, 10, 11,

		 // tree indices
		12, 13, 14, 15,
	};

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);														
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LoadTexture("textures/car_01.png", texture_car_01);
	LoadTexture("textures/car_02.png", texture_car_02);

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);



	// road
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 4));

	// curbs
	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 8));

	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(375, 0, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 8));

	// trees
	float treeGap = 62.5;
	float treeRightTranslation = 600;

		// left from bottom to top 
	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, treeGap, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 12));

	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, 250 + 2 * treeGap, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 12));

	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, 500 + 3 * treeGap, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 12));
	
		// right from bottom to top
	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(treeRightTranslation, treeGap, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 12));

	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(treeRightTranslation, 250 + 2 * treeGap, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 12));

	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(treeRightTranslation, 500 + 3 * treeGap, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 12));

	// cars
	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(-75, 0, 0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_car_01);

	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);


	myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(75, 0, 0.0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_car_02);

	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
	glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

	glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));

	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);



	glutSwapBuffers();
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

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

