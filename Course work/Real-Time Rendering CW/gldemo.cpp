

#include "stdafx.h"
#include <glew\glew.h>
#include <glew\wglew.h>
#include <freeglut\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include <CGImport\CGModel\CGModel.h>
#include <CGImport\Importers\CGImporters.h>
#include "helper_functions.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"
#include "CGPrincipleAxes.h"
#include "CGTexturedQuad.h"
#include "CGVBOModel.h"
#include "CGSphere.h"
#include "CGFont.h"
#include "CGSkybox.h"
#include "CGEarthScene.h"


using namespace std;
using namespace CoreStructures;

#pragma region Cube model data
// Per-vertex position vectors
static float cubeVertices[32] =
{
	-5.0f, 0.0f, -5.0f, 1.0f,
	-5.0f, 0.0f, 5.0f, 1.0f,
	5.0f, 0.0f, 5.0f, 1.0f,
	5.0f, 0.0f, -5.0f, 1.0f,
	-5.0f, -1.0f, -5.0f, 1.0f,
	-5.0f, -1.0f, 5.0f, 1.0f,
	5.0f, -1.0f, 5.0f, 1.0f,
	5.0f, -1.0f, -5.0f, 1.0f
};

// Per-vertex colours (RGBA) floating point values
static float cubeColours[32] =
{
	0.5, 0.6f, 0.8f, 1.0f,
	0.5, 0.6f, 0.8f, 1.0f,
	0.5, 0.6f, 0.8f, 1.0f,
	0.5, 0.6f, 0.8f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f
};

// 12 faces each with 3 vertices (each face forms a triangle) (36 indices total)
static unsigned short cubeVertexIndices[36] =
{
	0, 1, 2, // top (+y)
	0, 2, 3,
	4, 7, 5, // bottom (-y)
	5, 7, 6,
	0, 4, 1, // -x
	1, 4, 5,
	2, 7, 3, // +x
	2, 6, 7,
	0, 3, 4, // -z
	3, 7, 4,
	1, 5, 2, // +z
	2, 5, 6
};
#pragma endregion

#pragma region Pyramid model data

// Per-vertex position vectors
static float pyramidVertices[20] =
{
	0.0f, 2.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 0.0f, -1.0f, 1.0f,
	-1.0f, 0.0f, -1.0f, 1.0f,
	-1.0f, 0.0f, 1.0f, 1.0f
};

// Per-vertex colours (RGBA) floating point values
static float		pyramidColours[20] =
{
	0.5, 0.6f, 0.8f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f
};

// 5 faces each with 3 vertices (each face forms a triangle)
static unsigned short		pyramidVertexIndices[18] =
{
	0, 1, 2,
	0, 2, 3,
	0, 3, 4,
	0, 4, 1,
	3, 2, 4,
	1, 4, 2
};

#pragma endregion

#pragma region Scene variables and resources

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int								mouse_x, mouse_y;
bool							mDown = false;

GUClock							*mainClock = nullptr;

//
// Main scene resources
//
GUPivotCamera					*mainCamera = nullptr;
 
CGSkybox						*skybox = nullptr;
CGSphere						*sphereModel = nullptr;

GLuint							cubemapShader;

// Changes
static const GLuint				NUM_ROADS = 5;

CGTexturedQuad					*road[NUM_ROADS];
int								currentRoad;

CGFont							*font;
CoreStructures::GUMatrix4		fontViewMatrix;
CoreStructures::CGColorRGB		fontColour;

// Pyramid VBO example
GLuint							pyramidVertexBuffer;
GLuint							pyramidColourBuffer;
GLuint							pyramidIndexBuffer;
GLuint							pyramidVAO;

// Cube VBO example
GLuint							cubeVertexBuffer;
GLuint							cubeColourBuffer;
GLuint							cubeIndexBuffer;
GLuint							cubeVAO;
GLuint							pyramidShader, cubeShader;

// Texture for the spaceship
CGModel							*spaceShipModel = nullptr;
float							spaceShipRotation = 0.0f;

// Multi Texturing
GLuint							spaceShipTexture = 0;
GLuint							spaceShipTextureDetails = 0;
GLuint							spaceShipTextureShadow = 0;

GLuint							spaceShipShader = 0;

GLuint							snowTexture = 0;

// Unifom locations for spaceShipShader

// (Models a single directional light where lighting is done in world coordinates)
GLint							modelMatrixLocation = 0;
GLint							invTransposeMatrixLocation = 0;
GLint							viewProjectionMatrixLocation = 0;

GLint							lightDirectionLocation = 0;
GLint							lightDiffuseLocation = 0;
GLint							lightSpecularLocation = 0;
GLint							lightSpecExpLocation = 0;
GLint							cameraPosLocation = 0;

GLuint							spaceShipUniform = 0;
GLuint							spaceShipDetailsUniform = 0;
GLuint							spaceShipShadowUniform = 0;

//
// Earth scene
//
CGTexturedQuad					*texturedQuad = nullptr;
CGEarthScene					*earthScene = nullptr;
CGTexturedQuad					*earthQuad = nullptr;
bool							showEarthQuad = false;
bool							leftCtrlPressed = false;

// Angle to the Sun in the orbital plane of the Earth (the xz plane in the demo)
float							sunTheta = 0.0f;

//
// UBO model for CameraMatrixBlock (defined in example pyramid and cube shaders)
//

gu_byte							*cameraBuffer; // system memory buffer
GLint							cameraBufferSize;

GLuint							cameraUBO; // ID of buffer on GPU

// offset of start of uniform data
GLint							CameraMatrixBlock_offsets[2];

// mat4 column stride (column-major default) Note: can copy GUMatrix4 into either column or row major matrix
GLint							CameraMatrixBlock_matrixStrides[2];


// store uniform block indices for each shader that uses the block
// note: this could be an array for more complex applications
GLuint							pyramidCameraBlockIndex;
GLuint							cubeCameraBlockIndex;
#pragma endregion


#pragma region Function Prototypes

void init(void); // Main scene initialisation function
void update(void); // Main scene update function
void display(void); // Main scene render function

// Event handling functions
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyDown(unsigned char key, int x, int y);
void sysKeyDown(int key, int x, int y);

#pragma endregion



int _tmain(int argc, char* argv[]) {

	// Initialise GL Utility Toolkit and initialise COM so we can use the Windows Imaging Component (WIC) library
	glutInit(&argc, argv);
	CoInitialize(NULL);

	// Setup the OpenGL environment and initialise scene variables and resources
	init();

	// Setup and start the main clock
	mainClock = new GUClock();

	// Main event / render loop
	glutMainLoop();

	// Stop clock and report final timing data
	if (mainClock) {

		mainClock->stop();
		mainClock->reportTimingData();
		
		mainClock->release();
	}
	
	// Shut down COM and exit
	CoUninitialize();
	return 0;
}


#pragma region Initialisation, Update and Render

void init(void) {

	// Request an OpenGL 4.3 context with the Compatibility profile
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	// Setup OpenGL Display mode - include MSAA x4
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE | GLUT_SRGB);
	glutSetOption(GLUT_MULTISAMPLE, 16);

	// Setup window
	int windowWidth = 800;
	int windowHeight = 600;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("Real-Time Rendering Techniques");
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Register callback functions
	glutIdleFunc(update); // Main scene update function
	glutDisplayFunc(display); // Main render function
	glutMouseFunc(mouseButtonDown); // Mouse button handler
	glutMotionFunc(mouseMove); // Mouse move handler
	glutMouseWheelFunc(mouseWheel); // Mouse wheel event handler
	glutKeyboardFunc(keyDown); // Key down handler
	glutSpecialFunc(sysKeyDown);


	// Initialise GLEW library
	GLenum err = glewInit();

	// Ensure GLEW was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	} else {

		cout << "GLEW could not be initialised\n";
		throw;
	}
	
	// Example query OpenGL state (get version number)
	reportExtensions();
	reportContextVersion();


	//
	// Initiaise scene resources and variables
	//

	// Initialise OpenGL
	wglSwapIntervalEXT(0);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	// Setup main camera
	float viewportAspect = (float)windowWidth / (float)windowHeight;
	mainCamera = new GUPivotCamera(0.0f, 0.0f, 15.0f, 55.0f, viewportAspect, 0.1f);

	// Setup font
	font = new CGFont(wstring(L"Courier New"), 18);
	fontViewMatrix = GUMatrix4::orthographicProjection(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f);
	fontColour.r = 255;
	fontColour.g = 255;
	fontColour.b = 0;

	// Load Models
	spaceShipModel = new CGModel();
	spaceShipRotation = 0.0f;
	importOBJ(L"Resources\\Models\\spaceship.obj", spaceShipModel);

	// Load textures into appropriate texture units - these will be used to texture the spaceship model created above
	glActiveTexture(GL_TEXTURE1);
	spaceShipTexture = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\spaceship.png"), TextureGenProperties(GL_SRGB8_ALPHA8));
	spaceShipModel->setTextureForModel(spaceShipTexture);

	glActiveTexture(GL_TEXTURE2);
	spaceShipTextureDetails = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\spaceship_illumination.png"), TextureGenProperties(GL_SRGB8_ALPHA8));

	glActiveTexture(GL_TEXTURE3);
	spaceShipTextureShadow = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\spaceship_detail.gif"), TextureGenProperties(GL_SRGB8_ALPHA8));


	//spaceShipModel->setTextureForModel(spaceShipTextureShadow);
	glGenTextures(1, &spaceShipTexture);
	glGenTextures(2, &spaceShipTextureDetails);
	glGenTextures(3, &spaceShipTextureShadow);

	glActiveTexture(GL_TEXTURE0);

	//// Create the shader to render the earth model - this uses multi-texturing techniques to create a night and day effect
	//GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
	//	string("Resources\\Shaders\\basic_shader.vs"),
	//	string("Resources\\Shaders\\basic_shader.fs"),
	//	&spaceShipShader);

	// Setup uniform locations for shader

	spaceShipUniform = glGetUniformLocation(spaceShipShader, "spaceShipTexture");
	spaceShipDetailsUniform = glGetUniformLocation(spaceShipShader, "spaceShipTextureDetails");
	spaceShipShadowUniform = glGetUniformLocation(spaceShipShader, "spaceShipTextureShadow");

	modelMatrixLocation = glGetUniformLocation(spaceShipShader, "modelMatrix");

	invTransposeMatrixLocation = glGetUniformLocation(spaceShipShader, "invTransposeModelMatrix");

	viewProjectionMatrixLocation = glGetUniformLocation(spaceShipShader, "viewProjectionMatrix");

	lightDirectionLocation = glGetUniformLocation(spaceShipShader, "lightDirection");

	lightDiffuseLocation = glGetUniformLocation(spaceShipShader, "lightDiffuseColour");

	lightSpecularLocation = glGetUniformLocation(spaceShipShader, "lightSpecularColour");

	lightSpecExpLocation = glGetUniformLocation(spaceShipShader, "lightSpecularExponent");

	cameraPosLocation = glGetUniformLocation(spaceShipShader, "cameraPos");

	glUseProgram(spaceShipShader);

	glUniform1i(spaceShipUniform, 1);
	glUniform1i(spaceShipDetailsUniform, 2);
	glUniform1i(spaceShipShadowUniform, 3);

	glUseProgram(0);

	snowTexture = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\snow.jpg"), TextureGenProperties(GL_SRGB8_ALPHA8));

	texturedQuad = new CGTexturedQuad(wstring(L"..\\..\\Common\\Resources\\Textures\\bumblebee.png"));

	//
	// Setup earth scene
	//

	earthScene = new CGEarthScene();
	earthQuad = new CGTexturedQuad(earthScene->getEarthSceneTexture(), true);
	showEarthQuad = false;

	//
	// Load example road texture with different filtering properites
	//

	//// Point filtering
	//road[0] = new CGTexturedQuad(L"Resources\\Textures\\sand.jpg", TextureGenProperties(GL_COMPRESSED_SRGB, GL_NEAREST, GL_NEAREST, 1.0f, GL_REPEAT, GL_REPEAT, true));

	//// Bilinear filtering
	//road[1] = new CGTexturedQuad(L"Resources\\Textures\\sand.jpg", TextureGenProperties(GL_SRGB8_ALPHA8, GL_LINEAR, GL_LINEAR, 1.0f, GL_REPEAT, GL_REPEAT, true));

	//// Tri-linear filtering
	//road[2] = new CGTexturedQuad(L"Resources\\Textures\\road.bmp", TextureGenProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 1.0f, GL_REPEAT, GL_REPEAT, true));

	//// Anisotropic x2
	//road[3] = new CGTexturedQuad(L"Resources\\Textures\\road.bmp", TextureGenProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 2.0f, GL_REPEAT, GL_REPEAT, true));

	//// Anisotropic x8
	//road[4] = new CGTexturedQuad(L"Resources\\Textures\\road.bmp", TextureGenProperties(GL_SRGB8_ALPHA8, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, 8.0f, GL_REPEAT, GL_REPEAT, true));

	//currentRoad = 0;

#pragma region Setup VAO and VBOs for pyramid object

	// Setup VAO for pyramid object
	glGenVertexArrays(1, &pyramidVAO);
	glBindVertexArray(pyramidVAO);

	// Setup VBO for vertex position data
	glGenBuffers(1, &pyramidVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0); // attribute 0 gets data from bound VBO (so assign vertex position buffer to attribute 0)

	// Setup VBO for vertex colour data
	glGenBuffers(1, &pyramidColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColours), pyramidColours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0); // attribute 1 gets colour data

	// Enable vertex position and colour attribute arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	// Setup VBO for face index array
	glGenBuffers(1, &pyramidIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidVertexIndices), pyramidVertexIndices, GL_STATIC_DRAW);

	// Unbind pyramid VAO (or bind another VAO for another object / effect)
	// If we didn't do this, we may alter the bindings created above.
	glBindVertexArray(0);

#pragma endregion

#pragma region Setup VAO and VBOs for the cube object

	// setup VAO for cube object
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// setup VBO for vertex position data
	glGenBuffers(1, &cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// setup VBO for vertex colour data
	glGenBuffers(1, &cubeColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubeColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColours), cubeColours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	// setup VBO for cube face index array
	glGenBuffers(1, &cubeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeVertexIndices), cubeVertexIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
#pragma endregion

	// Setup main scene objects

	skybox = new CGSkybox(
		wstring(L"Resources\\Textures\\cubemap_snow\\"),
		wstring(L"snow"),
		wstring(L".png")
		);

	//sphereModel = new CGSphere(32, 64, 1.0f, GUVector4(1.0f, 1.0f, 1.0f, 1.0f), CG_RIGHTHANDED);

	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Cubemap\\cubemap3_vertex.txt"),
		string("Resources\\Shaders\\Cubemap\\cubemap3_fragment.txt"),
		&cubemapShader);

	//
	// Load example shaders
	//

	err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\pyramid_vertex.vs"),
		string("Resources\\Shaders\\pyramid_fragment.fs"),
		&pyramidShader);

	err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\cube_vertex.vs"),
		string("Resources\\Shaders\\cube_fragment.fs"),
		&cubeShader);

	//
	// UBO setup code for CameraMatrixBlock uniform block
	// For this example this is defined in example_vertex_shader.vs
	//

	cameraUBO = 0;

	// Get index of uniform block (blocks have indices just like normal uniform variables as discussed in previous lectures)
	pyramidCameraBlockIndex = glGetUniformBlockIndex(pyramidShader, "CameraMatrixBlock");

	// get actual size of block in bytes (necessary to allocate a buffer of the same size in system memory using malloc)
	glGetActiveUniformBlockiv(pyramidShader, pyramidCameraBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &cameraBufferSize);

	cameraBuffer = (gu_byte*)calloc(cameraBufferSize, 1);

	if (cameraBuffer) 
	{

		// create a new UBO object to represent the camera data block
		glGenBuffers(1, &cameraUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);


		// The general process of using a UBO is...
		// i) setup the data in the buffer in system memory
		// ii) copy this over to the UBO for use in our shaders

		// Before we can setup the data, we need to know how the memory in the buffer / uniform block is organised...

		// query indices of each uniform in CameraMatrixBlock - there are 2: 'viewMatrix' and 'projectionMatrix'

		const GLchar* CameraMatrixBlock_names[2] = {
			"CameraMatrixBlock.viewMatrix",
			"CameraMatrixBlock.projectionMatrix"
		};

		GLuint CameraMatrixBlock_indices[2] = { 0, 0 };

		glGetUniformIndices(
			pyramidShader,
			2,
			CameraMatrixBlock_names,
			CameraMatrixBlock_indices);

		// get structure data for CameraMatrixBlock in the pyramidShader, but the structure is the same for the cube shader also, so only do this once!
		glGetActiveUniformsiv(pyramidShader, 2, CameraMatrixBlock_indices, GL_UNIFORM_OFFSET, CameraMatrixBlock_offsets);

		// Bind cameraUBO to binding point 0...
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);

		// Bind the CameraMatrixBlock in cubeShader to binding point 0.  This means the uniform blocks in both shaders will see the same UBO data - so we set the data once and use multiple times in multiple shaders!
		cubeCameraBlockIndex = glGetUniformBlockIndex(cubeShader, "CameraMatrixBlock");
		glUniformBlockBinding(cubeShader, cubeCameraBlockIndex, 0);
	}
}


// Main scene update function (called by FreeGLUT's main event loop every frame) 
void update(void) {

	// Update clock
	mainClock->tick();

	// Update earthScene state
	if (earthScene)
		earthScene->update(mainClock->gameTimeDelta());

	// Update animation variables
	spaceShipRotation += 10.0f * float(mainClock->gameTimeDelta());

	// Redraw the screen
	display();

	// Update the window title to show current frames-per-second and seconds-per-frame data
	char timingString[256];
	sprintf_s(timingString, 256, "Real-Time Rendering Demo. Average fps: %.0f; Average spf: %f", mainClock->averageFPS(), mainClock->averageSPF() / 1000.0f);
	glutSetWindowTitle(timingString);
}


// Main scene rendering function
void display(void) {

	// Clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to the client area of the current window
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	
	//// Get view-projection transform as a GUMatrix4
	GUMatrix4 T = mainCamera->projectionTransform() * mainCamera->viewTransform(); //* GUMatrix4::rotationMatrix(-90.0f * gu_radian, 0.0f, 0.0f) * GUMatrix4::scaleMatrix(64.0f, 64.0f, 1.0f) * GUMatrix4::translationMatrix(-1.0f, -1.0f, 0.0f) * GUMatrix4::scaleMatrix(2.0f, 2.0f, 1.0f);

	GUMatrix4 cameraViewMatrix = mainCamera->viewTransform();
	GUMatrix4 cameraProjMatrix = mainCamera->projectionTransform();

	GUMatrix4 viewProjectionMatrix = cameraProjMatrix * cameraViewMatrix;

	// Setup the camera buffer with the latest camera transformation matrices
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

	memcpy(cameraBuffer + CameraMatrixBlock_offsets[0], &(cameraViewMatrix), 0x40);
	memcpy(cameraBuffer + CameraMatrixBlock_offsets[1], &(cameraProjMatrix), 0x40);

	glBufferData(GL_UNIFORM_BUFFER, cameraBufferSize, cameraBuffer, GL_DYNAMIC_DRAW);

	// Fixed function rendering (Compatability profile only) - use this since CGImport is written against OpenGL 2.1
	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf((const float*)mainCamera->projectionTransform().M);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf((const float*)mainCamera->viewTransform().M);
	glMultMatrixf((const float*)GUMatrix4::translationMatrix(0.0f, -0.15f, 0.0f).M);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spaceShipTexture);
	glBindTexture(GL_TEXTURE_2D, spaceShipTextureDetails);
	glBindTexture(GL_TEXTURE_2D, spaceShipTextureShadow);

	if (spaceShipModel)
	{
		//Modelling transform (incorporate the axial tilt (obliquity) of 23.44 degrees of the Earth)
		GUMatrix4 modelTransform = GUMatrix4::rotationMatrix(0.0f, 0.0f, 23.44f*gu_radian) * GUMatrix4::rotationMatrix(0.0f, spaceShipRotation*gu_radian, 0.0f);

		GUMatrix4 inverseTranspose = modelTransform.inverseTranspose();

		glUseProgram(spaceShipShader);

		// Get location of camera in world coords and set uniform in shader
		GUVector4 cameraPos = mainCamera->cameraLocation();
		glUniform4fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set view projection matrix (from camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (GLfloat*)&(modelTransform.M));

		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, (GLfloat*)&(inverseTranspose.M));

		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, (GLfloat*)&(T.M));

		glUniform4f(lightDirectionLocation, cosf(sunTheta*gu_radian), 0.0f, sinf(sunTheta*gu_radian), 0.0f); // World coordinate space vector

		glUniform4f(lightDiffuseLocation, 1.0f, 1.0f, 1.0f, 1.0f); // White diffuse light

		glUniform4f(lightSpecularLocation, 0.4f, 0.4f, 0.4f, 1.0f); // White specular light

		glUniform1f(lightSpecExpLocation, 8.0f); // Specular exponent / falloff
		glScalef(0.05f, 0.05f, 0.05f);
		spaceShipModel->renderTexturedModel();

		// Restore default OpenGL shaders
		glUseProgram(0);
	}
	glDisable(GL_TEXTURE_2D);

	//glEnable(GL_TEXTURE_2D);
	//glTranslatef(1000.0f, 1000.0f, 1000.0f);
	//road[currentRoad]->render(T);
	//
	////glRotatef(45.0, 0.0, 0.0, 1.0);

	//	 //Render currently selected filtering mode

	//	static const char *filterStrings[] = {
	//	"Point filtering",
	//	"Bi-linear filtering",
	//	"Tri-linear filtering",
	//	"Anisotropic filtering 2x",
	//	"Anisotropic filtering 8x" };

	//glDisable(GL_TEXTURE_2D);
	
	if (skybox)
		skybox->render(T);
	
	 //Render pyramid
	

	glUseProgram(pyramidShader);

	GUMatrix4 pyramidModelTransform = GUMatrix4::translationMatrix(4.0f, -4.0f, 0.0f);

	static GLint pyramidShader_modelMatrixLocation = glGetUniformLocation(pyramidShader, "modelMatrix");
	glUniformMatrix4fv(pyramidShader_modelMatrixLocation, 1, GL_FALSE, (const GLfloat*)&pyramidModelTransform);

	// Draw pyramid
	glBindVertexArray(pyramidVAO);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, (const GLvoid*)0);


	//
	// Render floor
	//

	glUseProgram(cubeShader);

	// Setup simple translation (model) matrix and pass to shader
	GUMatrix4 cubeModelTransform = GUMatrix4::translationMatrix(0.0f, -4.0f, 0.0f);

	static GLint cubeShader_modelMatrixLocation = glGetUniformLocation(cubeShader, "modelMatrix");
	glUniformMatrix4fv(cubeShader_modelMatrixLocation, 1, GL_FALSE, (const GLfloat*)&cubeModelTransform);


	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (const GLvoid*)0);

	glBindVertexArray(0);

	earthScene->render();

	if (showEarthQuad) {

		if (earthQuad)
			earthQuad->render(T);

	}
	else {

		if (texturedQuad)
			texturedQuad->render(T);
	}

	glutSwapBuffers();
}

#pragma endregion



#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	int dx = x - mouse_x;
	int dy = y - mouse_y;

	if (mainCamera)
		mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);
		
	mouse_x = x;
	mouse_y = y;
}


void mouseWheel(int wheel, int direction, int x, int y) {

	if (mainCamera) {

		if (direction<0)
			mainCamera->scaleCameraRadius(1.1f);
		else if (direction>0)
			mainCamera->scaleCameraRadius(0.9f);
	}
}

void keyDown(unsigned char key, int x, int y) {

	switch (key) {

	case '1':
		currentRoad = 0;
		break;

	case '2':
		currentRoad = 1;
		break;

	case '3':
		currentRoad = 2;
		break;

	case '4':
		currentRoad = 3;
		break;

	case '5':
		currentRoad = 4;
		break;

	case 'f':
		glutFullScreenToggle();
		break;

	case VK_SPACE:

		showEarthQuad = !showEarthQuad;
		break;

	}
}

void sysKeyDown(int key, int x, int y) {

	switch (key) {

	case GLUT_KEY_LEFT:
		sunTheta = sunTheta - 1.0f;
		break;

	case GLUT_KEY_RIGHT:
		sunTheta = sunTheta + 1.0f;
		break;
	}
}

#pragma endregion
