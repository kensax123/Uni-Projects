
#include "stdafx.h"
#include "CGEarthScene.h"
#include "CGSphere.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include <CoreStructures\GUVector4.h>
#include <CoreStructures\GUMatrix4.h>
#include <CGImport\CGModel\CGModel.h>
#include <CGImport\Importers\CGImporters.h>


using namespace std;
using namespace CoreStructures;


CGEarthScene::CGEarthScene() {

	earthModel = new CGSphere(32, 16, 1.0f, GUVector4(1.0f, 0.0f, 0.0f, 1.0f), CG_RIGHTHANDED);
	spaceShipModel = new CGModel();
	importOBJ(L"Resources\\Models\\spaceship.obj", spaceShipModel);

	earthCamera = new GUPivotCamera(0.0f, 0.0f, 5.0f, 55.0f, 1.0f, 0.1f);

	//
	// Setup textures for rendering the Earth model
	//

	/*glActiveTexture(GL_TEXTURE1);
	dayTexture = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\Blue Marble\\land_ocean_ice_cloud_2048.bmp"), TextureGenProperties(GL_SRGB8_ALPHA8));

	glActiveTexture(GL_TEXTURE2);
	nightTexture = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\Blue Marble\\land_ocean_ice_lights_2048.bmp"), TextureGenProperties(GL_SRGB8_ALPHA8));

	glActiveTexture(GL_TEXTURE3);
	cloudMaskTexture = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\Blue Marble\\Clouds.bmp"));

	glActiveTexture(GL_TEXTURE0);*/

	glActiveTexture(GL_TEXTURE1);
	spaceShipTexture = TextureLoader::wicLoadTexture(wstring(L"Resources\\Textures\\spaceship.png"), TextureGenProperties(GL_SRGB8_ALPHA8));
	spaceShipModel->setTextureForModel(spaceShipTexture);


	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\earth-multitexture.vs"),
		string("Resources\\Shaders\\earth-multitexture.fs"),
		&earthShader);


	// Setup uniform locations for shader

	dayTextureUniform = glGetUniformLocation(earthShader, "dayTexture");
	nightTextureUniform = glGetUniformLocation(earthShader, "nightTexture");
	maskTextureUniform = glGetUniformLocation(earthShader, "cloudMaskTexture");

	modelMatrixLocation = glGetUniformLocation(earthShader, "modelMatrix");

	invTransposeMatrixLocation = glGetUniformLocation(earthShader, "invTransposeModelMatrix");

	viewProjectionMatrixLocation = glGetUniformLocation(earthShader, "viewProjectionMatrix");

	lightDirectionLocation = glGetUniformLocation(earthShader, "lightDirection");

	lightDiffuseLocation = glGetUniformLocation(earthShader, "lightDiffuseColour");

	lightSpecularLocation = glGetUniformLocation(earthShader, "lightSpecularColour");

	lightSpecExpLocation = glGetUniformLocation(earthShader, "lightSpecularExponent");

	cameraPosLocation = glGetUniformLocation(earthShader, "cameraPos");


	// Set constant uniform data (uniforms that will not change while the application is running)
	// Note: Remember we need to bind the shader before we can set uniform variables!
	glUseProgram(earthShader);

	glUniform1i(dayTextureUniform, 1);
	glUniform1i(nightTextureUniform, 2);
	glUniform1i(maskTextureUniform, 3);

	glUseProgram(0);


	//
	// Setup FBO (which Earth rendering pass will draw into)
	//

	glGenFramebuffers(1, &demoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);


	//
	// Setup textures that will be drawn into through the FBO
	//

	// Setup colour buffer texture.
	// Note:  The texture is stored as linear RGB values (GL_RGBA8).  There is no need to pass a pointer to image data - we're going to fill in the image when we render the Earth scene at render time!
	glGenTextures(1, &fboColourTexture);
	glBindTexture(GL_TEXTURE_2D, fboColourTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 800, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// Setup depth texture

	glGenTextures(1, &fboDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 800, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//
	// Attach textures to the FBO
	//

	// Attach the colour texture object to the framebuffer object's colour attachment point #0
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		fboColourTexture,
		0);

	// Attach the depth texture object to the framebuffer object's depth attachment point
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		fboDepthTexture,
		0);


	//
	// Before proceeding make sure FBO can be used for rendering
	//

	GLenum demoFBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (demoFBOStatus != GL_FRAMEBUFFER_COMPLETE) {

		fboOkay = false;
		cout << "Could not successfully create framebuffer object to render texture!" << endl;

	}
	else {

		fboOkay = true;
		cout << "FBO successfully created" << endl;
	}

	// Unbind FBO for now! (Plug main framebuffer back in as rendering destination)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//
	// Setup demo / animation variables
	//

	sunTheta = 0.0f;
	earthTheta = 0.0f;
}


CGEarthScene::~CGEarthScene() {

}


// Accessor methods

CoreStructures::GUPivotCamera* CGEarthScene::getEarthSceneCamera() {

	return earthCamera;
}


GLuint CGEarthScene::getEarthSceneTexture() {

	return fboColourTexture;
}


float CGEarthScene::getSunTheta() {

	return sunTheta;
}


void CGEarthScene::updateSunTheta(float thetaDelta) {

	sunTheta += thetaDelta;
}


// Scene update
void CGEarthScene::update(const gu_seconds& timeDelta) {

	// Update rotation angle ready for next frame
	earthTheta += 15.0f * float(timeDelta);
}



// Rendering methods

void CGEarthScene::render() {

	if (!fboOkay)
		return; // Don't render anything if the FBO was not created successfully

	// Bind framebuffer object so all rendering redirected to attached images (i.e. our texture)
	glBindFramebuffer(GL_FRAMEBUFFER, demoFBO);

	// All rendering from this point goes to the bound textures (setup at initialisation time) and NOT the actual screen!!!!!

	// Clear the screen (i.e. the texture)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set viewport to specified texture size (see above)
	glViewport(0, 0, 800, 800);

	// Get view-projection transform as a CGMatrix4
	GUMatrix4 T = earthCamera->projectionTransform() * earthCamera->viewTransform();

	if (earthModel) {

		// Modelling transform
		GUMatrix4 modelTransform = GUMatrix4::rotationMatrix(0.0f, 0.0f, 23.44f*gu_radian) * GUMatrix4::rotationMatrix(0.0f, earthTheta*gu_radian, 0.0f);

		// Calculate inverse transpose of the modelling transform for correct transformation of normal vectors
		GUMatrix4 inverseTranspose = modelTransform.inverseTranspose();

		glUseProgram(earthShader);

		// Get the location of the camera in world coords and set the corresponding uniform in the shader
		GUVector4 cameraPos = earthCamera->cameraLocation();
		glUniform4fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set the model, view and projection matrix uniforms (from the camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (GLfloat*)&(modelTransform.M));
		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, (GLfloat*)&(inverseTranspose.M));
		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, (GLfloat*)&(T.M));

		// Set the light direction uniform vector in world coordinates based on the Sun's position
		glUniform4f(lightDirectionLocation, cosf(sunTheta*gu_radian), 0.0f, sinf(sunTheta*gu_radian), 0.0f);

		glUniform4f(lightDiffuseLocation, 1.0f, 1.0f, 1.0f, 1.0f); // white diffuse light
		glUniform4f(lightSpecularLocation, 0.4f, 0.4f, 0.4f, 1.0f); // white specular light
		glUniform1f(lightSpecExpLocation, 8.0f); // specular exponent / falloff


		earthModel->render();

		// Restore default OpenGL shaders (Fixed function operations)
		glUseProgram(0);
	}

	if (spaceShipModel)
	{
		//Modelling transform (incorporate the axial tilt (obliquity) of 23.44 degrees of the Earth)
		GUMatrix4 modelTransform = GUMatrix4::rotationMatrix(0.0f, 0.0f, 23.44f*gu_radian) * GUMatrix4::rotationMatrix(0.0f, earthTheta*gu_radian, 0.0f);
		GUMatrix4 inverseTranspose = modelTransform.inverseTranspose();

		glUseProgram(earthShader);

		// Get location of camera in world coords and set uniform in shader
		GUVector4 cameraPos = earthCamera->cameraLocation();
		glUniform4fv(cameraPosLocation, 1, (GLfloat*)&cameraPos);

		// Set view projection matrix (from camera data obtained above)
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, (GLfloat*)&(modelTransform.M));

		glUniformMatrix4fv(invTransposeMatrixLocation, 1, GL_FALSE, (GLfloat*)&(inverseTranspose.M));

		glUniformMatrix4fv(viewProjectionMatrixLocation, 1, GL_FALSE, (GLfloat*)&(T.M));

		glUniform4f(lightDirectionLocation, cosf(sunTheta*gu_radian), 0.0f, sinf(sunTheta*gu_radian), 0.0f); // World coordinate space vector

		glUniform4f(lightDiffuseLocation, 1.0f, 1.0f, 1.0f, 1.0f); // White diffuse light

		glUniform4f(lightSpecularLocation, 1.0f, 1.0f, 1.0f, 1.0f); // White specular light

		glUniform1f(lightSpecExpLocation, 8.0f); // Specular exponent / falloff
		//glScalef(0.05f, 0.05f, 0.05f);
		//spaceShipModel->renderTexturedModel();

		// Restore default OpenGL shaders
		glUseProgram(0);
	}

	// Set OpenGL to render to the MAIN framebuffer (ie. the screen itself!!)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
