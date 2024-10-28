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

#include <vector>
#include <utility>


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
	texture_curbs,
	texture_lost,
	texture_won;

glm::mat4
	myMatrix, resizeMatrix, controlCarMatrix;

float xMin = -400, xMax = 400, yMin = -500, yMax = 500;
float moveX = 0, moveY = 0, movementStep = 2, i = 0, forwardStep = 0.5;
float carSizeX = 100, carSizeY = 200, tolerance = 10; // used for tolerance in collision detection

bool keyUpPressed = false,
	 keyDownPressed = false,
	 keyLeftPressed = false,
	 keyRightPressed = false,
	 hasStarted = false;

int gameState = 0;

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

void SpecialKeyPressed(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			hasStarted = true;
			keyUpPressed = true;
			break;
		case GLUT_KEY_DOWN:
			hasStarted = true;
			keyDownPressed = true;
			break;
		case GLUT_KEY_LEFT:
			hasStarted = true;
			keyLeftPressed = true;
			break;
		case GLUT_KEY_RIGHT:
			hasStarted = true;
			keyRightPressed = true;
			break;
	}
}

void SpecialKeyReleased(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			keyUpPressed = false;
			break;
		case GLUT_KEY_DOWN:
			keyDownPressed = false;
			break;
		case GLUT_KEY_LEFT:
			keyLeftPressed = false;
			break;
		case GLUT_KEY_RIGHT:
			keyRightPressed = false;
			break;
	}
}

void NormalKeyPressed(unsigned char key, int x, int y) {
	switch (key) {
		case 'w':
		case 'W':
			hasStarted = true;
			keyUpPressed = true;
			break;
		case 's':
		case 'S':
			hasStarted = true;
			keyDownPressed = true;
			break;
		case 'a':
		case 'A':
			hasStarted = true;
			keyLeftPressed = true;
			break;
		case 'd':
		case 'D':
			hasStarted = true;
			keyRightPressed = true;
			break;
		case 'r':
		case 'R':
			hasStarted = false;
			gameState = 0;
			i = 0;
			moveX = 0;
			moveY = 0;
			break;
	}

	if (key == 27)
		exit(0);
}

void NormalKeyReleased(unsigned char key, int x, int y) {
	switch (key) {
		case 'w':
		case 'W':
			keyUpPressed = false;
			break;
		case 's':
		case 'S':
			keyDownPressed = false;
			break;
		case 'a':
		case 'A':
			keyLeftPressed = false;
			break;
		case 'd':
		case 'D':
			keyRightPressed = false;
			break;
	}
}

void UpdateCarPosition() {
	moveX += (keyRightPressed - keyLeftPressed) * movementStep;
	moveY += (keyUpPressed - keyDownPressed) * movementStep;
}

void MoveForward() {
	if (hasStarted)
		i += forwardStep;
}

struct Point {
	float x = 0.0f;
	float y = 0.0f;
};

struct CarCoordinates
{
	Point bottomLeft, topRight;
};

std::pair<CarCoordinates, CarCoordinates> getCarsCoordinates() {
	CarCoordinates dynamicCar = {
		{ moveX + 75 - carSizeX / 2, moveY - carSizeY / 2 - 375 },
		{ moveX + 75 + carSizeX / 2, moveY + carSizeY / 2 - 375 }
	};

	CarCoordinates staticCar = {
		{ 25, i - carSizeY / 2 - 100 },
		{ 175, i + carSizeY / 2 - 100 } 
	};

	return { dynamicCar, staticCar };
}

void CheckCollisions() {
	std::pair<CarCoordinates, CarCoordinates> coordinates = getCarsCoordinates();
	CarCoordinates dynamicCarCoordinates = coordinates.first;
	CarCoordinates staticCarCoordinates = coordinates.second;
	

	// check for curb collisions and lower/upper bound limit
	if (dynamicCarCoordinates.bottomLeft.x <= -175 || dynamicCarCoordinates.topRight.x >= 175 || dynamicCarCoordinates.bottomLeft.y <= -500 || dynamicCarCoordinates.topRight.y >= 500)
		gameState = -1;

	// check for car collisions
		// checks if top right corner is inside the other car
	if (dynamicCarCoordinates.topRight.x >= staticCarCoordinates.bottomLeft.x + tolerance && dynamicCarCoordinates.topRight.x <= staticCarCoordinates.topRight.x - tolerance &&
		dynamicCarCoordinates.topRight.y >= staticCarCoordinates.bottomLeft.y + tolerance && dynamicCarCoordinates.topRight.y <= staticCarCoordinates.topRight.y - tolerance) {
		gameState = -1;
	}

		// checks if bottom right corner is inside the other car
	if (dynamicCarCoordinates.topRight.x >= staticCarCoordinates.bottomLeft.x + tolerance && dynamicCarCoordinates.topRight.x <= staticCarCoordinates.topRight.x - tolerance &&
		dynamicCarCoordinates.bottomLeft.y >= staticCarCoordinates.bottomLeft.y + tolerance && dynamicCarCoordinates.bottomLeft.y <= staticCarCoordinates.topRight.y - tolerance) {
		gameState = -1;
	}
}

void CheckWinCondition() {
	std::pair<CarCoordinates, CarCoordinates> coordinates = getCarsCoordinates();
	CarCoordinates dynamicCarCoordinates = coordinates.first;
	CarCoordinates staticCarCoordinates = coordinates.second;

	if (dynamicCarCoordinates.bottomLeft.x >= 20 && dynamicCarCoordinates.bottomLeft.y >= staticCarCoordinates.topRight.y)
		gameState = 1;
}

void CreateVBO(void)
{
	GLfloat Vertices[] = {
		// car vertices - left
		-50.0f,   100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		-50.0f,  -100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 50.0f,  -100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 50.0f,   100.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 1.0f,

		// road vertices
		-175.0f,   500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		-175.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 175.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 175.0f,   500.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,   1.0f, 1.0f,

		// curbs vertices - left
		-200.0f,   500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f,
		-200.0f,  -500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f,
		-175.0f,  -500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f,
		-175.0f,   500.0f, 0.0f, 1.0f,    0.5f, 0.5f, 0.5f,   1.0f, 1.0f,

		// tree - bottom left
		-375.0f,  -250.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		-375.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
		-225.0f,  -500.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-225.0f,  -250.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

		// end screen
		-400.0f,   500.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
		-400.0f,  -500.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		 400.0f,  -500.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
		 400.0f,   500.0f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
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

		 // end screen
		16, 17, 18, 19,
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
	LoadTexture("textures/road.jpg", texture_asphalt);
	LoadTexture("textures/tree.png", texture_tree);
	LoadTexture("textures/lost.png", texture_lost);
	LoadTexture("textures/won.png", texture_won);

	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	CheckCollisions();
	CheckWinCondition();

	switch (gameState) {
		case -1:
			myMatrix = resizeMatrix;
			glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_lost);

			glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 16));

			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);

			break;
		case  1:
			myMatrix = resizeMatrix;
			glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_won);

			glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 16));

			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);

			break;
		case  0:
			MoveForward();

			// road
			myMatrix = resizeMatrix;
			glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_asphalt);

			glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 4));

			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);



			// curbs
			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 8));

			myMatrix = resizeMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(375, 0, 0));
			glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(sizeof(GLuint) * 8));



			// trees
			float treeGap = 62.5;
			float treeRightTranslation = 600;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_tree);

			glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

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

			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);



			// cars
				// left
			UpdateCarPosition();

			controlCarMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(moveX, moveY, 0));
			myMatrix = resizeMatrix * controlCarMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(75, -375, 0));
			glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_car_01);

			glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));

			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);

				// right
			controlCarMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, i, 0));
			myMatrix = resizeMatrix * controlCarMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(75, -100, 0.0));
			glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_car_02);

			glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 1);

			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, (void*)(0));

			glBindTexture(GL_TEXTURE_2D, 0);
			glUniform1i(glGetUniformLocation(ProgramId, "hasTexture"), 0);

			break;
	}
	
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

	glutIdleFunc(RenderFunction);

	glutSpecialFunc(SpecialKeyPressed);
	glutSpecialUpFunc(SpecialKeyReleased);

	glutKeyboardFunc(NormalKeyPressed);
	glutKeyboardUpFunc(NormalKeyReleased);

	glutCloseFunc(Cleanup);

	glutMainLoop();

	return 0;
}
