
#pragma once

#include <CoreStructures\GUMemory.h>
#include <CoreStructures\GUObject.h>
#include <glew\glew.h>
#include <glew\wglew.h>
#include <CoreStructures\GUPivotCamera.h>

class CGSphere;


class CGEarthScene : public CoreStructures::GUObject {

private:

	CGSphere						*earthModel;
	CGModel							*spaceShipModel = nullptr;
	GLuint							spaceShipTexture = 0;
	float							spaceShipRotation = 0.0f;
	// Move around the earth with a seperate camera to the main scene camera
	CoreStructures::GUPivotCamera	*earthCamera;

	// Textures for multi-texturing the earth model
	GLuint							dayTexture;
	GLuint							nightTexture;
	GLuint							cloudMaskTexture;

	// Shader for multi-texturing the earth
	GLuint							earthShader;


	// Unifom locations for earthShader

	// Texture uniforms
	GLuint							dayTextureUniform;
	GLuint							nightTextureUniform;
	GLuint							maskTextureUniform;

	// Camera uniforms
	GLint							modelMatrixLocation;
	GLint							invTransposeMatrixLocation;
	GLint							viewProjectionMatrixLocation;

	// Directional light uniforms
	GLint							lightDirectionLocation;
	GLint							lightDiffuseLocation;
	GLint							lightSpecularLocation;
	GLint							lightSpecExpLocation;
	GLint							cameraPosLocation;


	//
	// Animation state
	//
	float							sunTheta; // Angle to the Sun in the orbital plane of the Earth (the xz plane in the demo)
	float							earthTheta;


	//
	// Framebuffer Object (FBO) variables
	//

	// Actual FBO
	GLuint							demoFBO;

	// Colour texture to render into
	GLuint							fboColourTexture;

	// Depth texture to render into
	GLuint							fboDepthTexture;

	// Flag to indicate that the FBO is valid
	bool							fboOkay;

public:

	CGEarthScene();
	~CGEarthScene();

	// Accessor methods
	CoreStructures::GUPivotCamera* getEarthSceneCamera();
	GLuint getEarthSceneTexture();
	float getSunTheta();
	void updateSunTheta(float thetaDelta);

	// Scene update
	void update(const CoreStructures::gu_seconds& timeDelta);

	// Rendering methods
	void render();
};
