
#include "stdafx.h"
#include "CGSkybox.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"


using namespace std;
using namespace CoreStructures;


#pragma region Skybox model

// Packed vertex buffer for skybox model.  Vertces duplicated so each vertex on each face has its own normal
static float skyboxPositionArray[] = {

	-5.0f, -5.0f, -5.0f, 1.0f,
	5.0f, -5.0f, -5.0f, 1.0f,
	5.0f, 5.0f, -5.0f, 1.0f,
	-5.0f, 5.0f, -5.0f, 1.0f,

	-5.0f, -5.0f, 5.0f, 1.0f,
	-5.0f, 5.0f, 5.0f, 1.0f,
	5.0f, 5.0f, 5.0f, 1.0f,
	5.0f, -5.0f, 5.0f, 1.0f,

	-5.0f, -5.0f, -5.0f, 1.0f,
	-5.0f, 5.0f, -5.0f, 1.0f,
	-5.0f, 5.0f, 5.0f, 1.0f,
	-5.0f, -5.0f, 5.0f, 1.0f,

	5.0f, -5.0f, -5.0f, 1.0f,
	5.0f, -5.0f, 5.0f, 1.0f,
	5.0f, 5.0f, 5.0f, 1.0f,
	5.0f, 5.0f, -5.0f, 1.0f,

	5.0f, 5.0f, -5.0f, 1.0f,
	5.0f, 5.0f, 5.0f, 1.0f,
	-5.0f, 5.0f, 5.0f, 1.0f,
	-5.0f, 5.0f, -5.0f, 1.0f,

	5.0f, -5.0f, -5.0f, 1.0f,
	-5.0f, -5.0f, -5.0f, 1.0f,
	-5.0f, -5.0f, 5.0f, 1.0f,
	5.0f, -5.0f, 5.0f, 1.0f
};


static float skyboxNormalArray[] = {

	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,

	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};


#pragma endregion


CGSkybox::CGSkybox(const wstring& cubemapPath, const wstring& cubemapName, const wstring& cubemapExtension) {

	// Load shader
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(string("Resources\\Shaders\\Skybox\\skybox_vertex.txt"), string("Resources\\Shaders\\Skybox\\skybox_fragment.txt"), &skyboxShader);


	// Setup VAO
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	// Setup VBO for vertex position data
	glGenBuffers(1, &skyboxVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxPositionArray), skyboxPositionArray, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

	// Setup VBO for normal vector data
	glGenBuffers(1, &skyboxNormalVectorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxNormalVectorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxNormalArray), skyboxNormalArray, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 0, (const GLvoid*)0);

	// Enable vertex buffers
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2);


	// Unbind VAO when finished setup
	glBindVertexArray(0);


	//
	// Load cubemap texture
	//

#if 1

	// This version uses GL_MIRRORED_REPEAT to help eliminate seam edges in the cube map and skybox rendering

	texture = TextureLoader::loadCubeMapTexture(
		cubemapPath, cubemapName, cubemapExtension,
		GL_SRGB8_ALPHA8,
		GL_LINEAR,
		GL_LINEAR,
		4.0f,
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_MIRRORED_REPEAT,
		false);

#else

	// This version uses GL_REPEAT which introduces cubemap sampling artefacts at the corners and edges of the skybox

	texture = TextureLoader::loadCubeMapTexture(
		cubemapPath, cubemapName, cubemapExtension,
		GL_LINEAR,
		GL_LINEAR,
		4.0f,
		GL_REPEAT,
		GL_REPEAT,
		GL_REPEAT,
		false);

#endif


}


CGSkybox::~CGSkybox() {

	// Unbind VAO
	glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Delete GL resources
	glDeleteBuffers(1, &skyboxVertexBuffer);
	glDeleteBuffers(1, &skyboxNormalVectorBuffer);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteShader(skyboxShader);
	glDeleteTextures(1, &texture);
}


GLuint CGSkybox::skyboxTexture() {

	return texture;
}


void CGSkybox::setSkyboxTexture(const GLuint newTexture) {

	if (newTexture == 0)
		return;

	if (texture)
		glDeleteTextures(1, &texture);

	texture = newTexture;
}


void CGSkybox::render(const GUMatrix4& T) {

	static GLint mvpLocation = glGetUniformLocation(skyboxShader, "modelViewProjectionMatrix");

	glUseProgram(skyboxShader);
	glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*)&(T.M));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// Draw skybox directly - no indexing used
	glBindVertexArray(skyboxVAO);
	glDrawArrays(GL_QUADS, 0, 24);

	// Unbind VAO once drawing complete
	glBindVertexArray(0);
}
