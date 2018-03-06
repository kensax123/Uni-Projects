#include <iostream>
#include <stdlib.h>
#include <vector>
#include <glew\glew.h>
#include <FreeImage\FreeImagePlus.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include <wincodec.h>
#include "texture_loader.h"
#include "stdafx.h"

using namespace std;
using namespace CoreStructures;

//Bear Movement
float bearX = 0.0f;

//Bear Speed
float bearSpeed = 0.02;

bool keyLeftPressed = false;
bool keyRightPressed = false;

// Textures
GLuint backgroundTexture = 0;

//Tree Management
GLuint leavesTexture = 0;

static vector<float> treesLocation;

void drawTree(float xLoc, float yLoc);
void generateTrees(int noOfTrees, float startX, float startY, float maxX, float maxY);
void makeTrees();

//Background Management
void drawBackground();

//Scenery
void drawScenery();

//Land
void drawLand();

//Water Management
void drawWaterBackground();
void drawWaterTrans();

//Cloud Management
float cloud_1 = 0.0f;
float cloud_2 = 0.0f;
float cloud_3 = 0.0f;

void drawCloud(const unsigned int numVertices, float xLoc, float yLoc);

//Fish Management
void drawFish(const unsigned int numVertices, float xLoc, float yLoc);

//Bear Management
void drawBear(const unsigned int numVertices, float xLoc, float yLoc);

//Event Management
float theta = 0.0f;

// Function Prototypes
void init(int argc, char* argv[]);
void display(void);
float randf_range(float a, float b);

void update(void);
void displayTransformMatrix(float *M);
void specialKeyDown(int key, int x, int y);
void specialKeyUp(int key, int x, int y);

int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	// Shut down COM
	shutdownCOM();

	return 0;
}

void init(int argc, char* argv[]) {

	// Initialise COM so we can use Windows Imaging Component
	initCOM();


	// Initialise FreeGLUT
	glutInit(&argc, argv);

	// Setup OpenGL window properties
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	// Create window
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Texture Mapping Demo");

	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	}
	else {

		cout << "GLEW could not be initialised\n";
		throw;
	}

	// Example query OpenGL state (get version number)
	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";


	// Initialise OpenGL state

	// Setup colour to clear the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set viewplane size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);

	// Load texture
	backgroundTexture = fiLoadTexture("..\\..\\..\\Common\\Resources\\Textures\\mt_fuji.jpg");
	leavesTexture = fiLoadTexture("..\\..\\..\\Common\\Resources\\Textures\\bush.jpg");

	//Generating Trees
	generateTrees(30, 0.75, 0.0, 0.9, 0.08);

	//Enable Blending and Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Register callback functions
	glutDisplayFunc(display);
	glutSpecialFunc(specialKeyDown);
	glutSpecialUpFunc(specialKeyUp);
	glutIdleFunc(update);

	// Clear all previous transformations
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set line width
	glLineWidth(2.0f);
}

void update(void)
{
	if (keyLeftPressed)
	{
		bearX -= bearSpeed;
	}

	if (keyRightPressed)
	{
		bearX += bearSpeed;
	}

	cloud_1 -= 0.01;
	if (cloud_1 < -3.5)
		cloud_1 = 0;
	cloud_2 -= 0.005;
	if (cloud_2 < -4.0)
		cloud_2 = 0;
	cloud_3 -= 0.0075;
	if (cloud_3 < -4.5)
		cloud_3 = 0;
	glutPostRedisplay();
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Draw here
	drawBackground();
	drawScenery();
	drawLand();
	makeTrees();
	drawWaterBackground();
	drawFish(360, -0.5, -0.8);
	drawFish(360, 0.5, -0.7);
	drawFish(360, -0.0, -0.9);
	drawWaterTrans();

	drawCloud(360, 1.5 + cloud_1, 0.5);
	drawCloud(360, 2.0 + cloud_2, 0.5);
	drawCloud(360, 2.5 + cloud_3, 0.7);
	
	glTranslatef(bearX, 0.0f, 0.0f);
	drawBear(360, 0, 0);
	
	glutSwapBuffers();
}

void specialKeyDown(int key, int x, int y) {

	switch (key) {

	case GLUT_KEY_LEFT:
		keyLeftPressed = true;
		break;

	case GLUT_KEY_RIGHT:
		keyRightPressed = true;
		break;
	}
}

void specialKeyUp(int key, int x, int y) {

	switch (key) {

	case GLUT_KEY_LEFT:
		keyLeftPressed = false;
		break;

	case GLUT_KEY_RIGHT:
		keyRightPressed = false;
		break;
	}
}

//Tree Functions
void drawTree(float x, float y)
{
	//Leaves
	glBindTexture(GL_TEXTURE_2D, leavesTexture);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);

	glTexCoord2f(1.0f, 1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(x - 0.8f, y + 0.2);

	glTexCoord2f(-1.0f, 0.0f);
	glVertex2f(x - 0.9f, y - 0.1);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(x - 0.7f, y - 0.1);

	glEnd();

	//Trunk
	glBegin(GL_QUAD_STRIP);

	glColor3f(0.647059f, 0.164706f, 0.164706f);
	glVertex2f(x - 0.78f, y - 0.1);
	glVertex2f(x - 0.83f, y - 0.1);
	glVertex2f(x - 0.78f, y - 0.2);
	glVertex2f(x - 0.83f, y - 0.2);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void generateTrees(int noOfTrees, float startX, float startY, float maxX, float maxY)
{
	treesLocation.push_back(startX);
	treesLocation.push_back(startY);
	for (int i = 0; i < noOfTrees - 1; i++)
	{
		//Finding trees location
		float randX = randf_range(-maxX, maxX);
		float randY = randf_range(-maxY, maxY);
		treesLocation.push_back(randX + startX);
		treesLocation.push_back(randY + startY);
	}
}

void makeTrees()
{
	for (int i = 0; i < treesLocation.size(); i += 2)
	{
		float x = treesLocation[i];
		float y = treesLocation[i + 1];

		drawTree(x, y);
	}
}

//Background Functions
void drawBackground()
{
	glBegin(GL_TRIANGLE_FAN);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);

	glEnd();
}

//Scenery Functions
void drawScenery(void)
{

	glBindTexture(GL_TEXTURE_2D, backgroundTexture);

	glEnable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, -1.5f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.5f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

//Land Functions
void drawLand(void)
{
	glBegin(GL_QUAD_STRIP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-1.0f, -0.08f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.0f, -0.08f);

	glColor3f(0.36f, 0.25f, 0.20f);
	glVertex2f(-1.0f, -1.0f);

	glColor3f(0.36f, 0.25f, 0.20f);
	glVertex2f(1.0f, -1.0f);

	glEnd();
}

//Water Functions
void drawWaterBackground(void)
{
	glBegin(GL_QUAD_STRIP);

	glColor4f(0.752941f, 0.752941f, 0.752941f, 1.0f);
	glVertex2f(-1.0f, -0.6f);

	glColor4f(0.752941f, 0.752941f, 0.752941f, 1.0f);
	glVertex2f(1.0f, -0.6f);

	glColor4f(0.752941f, 0.752941f, 0.752941f, 1.0f);
	glVertex2f(-1.0f, -1.0f);

	glColor4f(0.752941f, 0.752941f, 0.752941f, 1.0f);
	glVertex2f(1.0f, -1.0f);

	glEnd();
}

void drawWaterTrans(void)
{
	glBegin(GL_QUAD_STRIP);

	glColor4f(0.0f, 0.0f, 1.0f, 0.3f);
	glVertex2f(-1.0f, -0.61f);

	glColor4f(0.0f, 0.0f, 1.0f, 0.3f);
	glVertex2f(1.0f, -0.61f);

	glColor4f(0.0f, 0.0f, 1.0f, 0.3f);
	glVertex2f(-1.0f, -1.0f);

	glColor4f(0.0f, 0.0f, 1.0f, 0.3f);
	glVertex2f(1.0f, -1.0f);

	glEnd();
}

//Cloud Functions
void drawCloud(const unsigned int numVertices, float xLoc, float yLoc)
{

	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 0.90);

	float radius = 0.08f;

	//draw the points around the circumference.
	for (unsigned int i = 0; i <= numVertices; ++i)
	{
		float x = cosf(i*2.0*gu_pi / numVertices) * radius;
		float y = sinf(i*2.0*gu_pi / numVertices) * radius;

		glVertex2f(x + xLoc, y + yLoc);
	}
	glEnd();


	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 0.90);

	float radius2 = 0.08f;

	//draw the points around the circumference.
	for (unsigned int i = 0; i <= numVertices; ++i)
	{
		float x = cosf(i*2.0*gu_pi / numVertices) * radius2;
		float y = sinf(i*2.0*gu_pi / numVertices) * radius2;

		glVertex2f(x + 0.10 + xLoc, y + 0.05 + yLoc);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glColor4f(1.0f, 1.0f, 1.0f, 0.90);

	float radius3 = 0.08f;

	//draw the points around the circumference.
	for (unsigned int i = 0; i <= numVertices; ++i)
	{
		float x = cosf(i*2.0*gu_pi / numVertices) * radius3;
		float y = sinf(i*2.0*gu_pi / numVertices) * radius3;

		glVertex2f(x + 0.18 + xLoc, y - 0.03 + yLoc);
	}
	glEnd();

	glBegin(GL_POLYGON);

	float radius4 = 0.08f;
	glColor4f(1.0f, 1.0f, 1.0f, 0.90);

	//draw the points around the circumference.
	for (unsigned int i = 0; i <= numVertices; ++i)
	{
		float x = cosf(i*2.0*gu_pi / numVertices) * radius4;
		float y = sinf(i*2.0*gu_pi / numVertices) * radius4;

		glVertex2f(x + 0.23 + xLoc, y + 0.03 + yLoc);
	}
	glEnd();
}

//Fish Functions
void drawFish(const unsigned int numVertices, float xLoc, float yLoc)
{
	//Fish Body
	glBegin(GL_POLYGON);
	glColor4f(0.0f, 0.0f, 1.0f, 0.90);

	float radius = 0.03f;

	for (unsigned int i = 0; i <= numVertices; ++i)
	{
		float x = cosf(i*2.0*gu_pi / numVertices) * radius * 3;
		float y = sinf(i*2.0*gu_pi / numVertices) * radius;

		glVertex2f(x + xLoc, y + yLoc);
	}
	glEnd();

	//Fish Tail
	glBegin(GL_TRIANGLES);

	glColor4f(0.0f, 0.0f, 1.0f, 0.90);
	glVertex2f(0.0f + xLoc, 0.0f + yLoc);

	glColor4f(0.0f, 0.0f, 1.0f, 0.90);
	glVertex2f(0.10f + xLoc, 0.05f + yLoc);

	glColor4f(0.0f, 0.0f, 1.0f, 0.90);
	glVertex2f(0.10f + xLoc, -0.05f + yLoc);

	glEnd();

	//Fish Eye
	glBegin(GL_POINTS);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0);
	glVertex2f(-0.07f + xLoc, 0.0f + yLoc);

	glEnd();

}

//Bear Functions
void drawBear(const unsigned int numVertices, float xLoc, float yLoc)
{

	//Bear's Body
	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.0f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.25f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.25f, 0.0f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.0f, 0.0f);

	glEnd();


	//Bear's Belly
	glBegin(GL_QUADS);

	glColor3f(0.59f, 0.41f, 0.31f);
	glVertex2f(-0.05f, -0.2f);

	glColor3f(0.59f, 0.41f, 0.31f);
	glVertex2f(-0.20f, -0.2f);

	glColor3f(0.59f, 0.41f, 0.31f);
	glVertex2f(-0.20f, -0.15f);

	glColor3f(0.59f, 0.41f, 0.31f);
	glVertex2f(-0.05f, -0.15f);

	glEnd();


	//Bear's Tail
	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.25f, 0.0f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.28f, 0.0f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.28f, -0.05f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.25f, -0.05f);

	glEnd();


	//Bear's Legs
	//Front Legs
	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.01f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.05f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.05f, -0.3f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.01f, -0.3f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.06f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.10f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.10f, -0.3f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.06f, -0.3f);

	glEnd();


	//Back Legs
	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.20f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.24f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.24f, -0.3f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.20f, -0.3f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.15f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.19f, -0.2f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.19f, -0.3f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.15f, -0.3f);

	glEnd();


	//Bear's Face
	glBegin(GL_TRIANGLE_STRIP);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.10f, 0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.10f, -0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.10f, 0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.10f, -0.10f);

	glEnd();


	glBegin(GL_POLYGON);

	glColor3f(0.59f, 0.41f, 0.31f);

	float radius = 0.05f;

	for (unsigned int i = 0; i <= numVertices; ++i)
	{
		float x = cosf(i*2.0*gu_pi / numVertices) * radius;
		float y = sinf(i*2.0*gu_pi / numVertices) * radius;

		glVertex2f(x, y - 0.02);
	}
	glEnd();


	//Bear's Left Ear
	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.02f, 0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.08f, 0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.08f, 0.17f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(-0.02f, 0.17f);

	glEnd();


	//Bear's Right Ear
	glBegin(GL_QUADS);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.02f, 0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.08f, 0.10f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.08f, 0.17f);

	glColor3f(0.35f, 0.16f, 0.14f);
	glVertex2f(0.02f, 0.17f);

	glEnd();


	//Bear's Left Eye
	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.04f, 0.02f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.06f, 0.02f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.06f, 0.05f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.04f, 0.05f);

	glEnd();


	//Bear's Right Eye
	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.04f, 0.02f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.06f, 0.02f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.06f, 0.05f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.04f, 0.05f);

	glEnd();


	//Bear's Nose
	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.01f, -0.01f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.01f, -0.01f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(-0.01f, 0.01f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.01f, 0.01f);

	glEnd();



}

float randf_range(float a, float b)
{
	float r = ((float)rand()) / (float)RAND_MAX;

	return a + (r * (b - a));
}
