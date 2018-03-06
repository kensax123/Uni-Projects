
#pragma once

#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <glew\glew.h>


struct TextureGenProperties;


// Model a simple textured quad oriented to face along the +z axis (so the textured quad faces the viewer in (right-handed) eye coordinate space.  The quad is modelled using VBOs and VAOs and rendered using the basic texture shader in Resources\Shaders\basic_texture.vs and Resources\Shaders\basic_texture.fs

class CGTexturedQuad : public CoreStructures::GUObject {

private:

	GLuint					quadVertexArrayObj;

	GLuint					quadVertexBuffer;
	GLuint					quadTextureCoordBuffer;

	GLuint					quadShader;

	GLuint					texture;

	//
	// Private API
	//

	void loadShader();
	void setupVAO(bool invertV);


	//
	// Public API
	//

public:

	CGTexturedQuad(const std::wstring& texturePath, bool invertV = false);
	CGTexturedQuad(GLuint initTexture, bool invertV = false);
	CGTexturedQuad(const std::wstring& texturePath, const TextureGenProperties& textureProperties, bool invertV = false);

	~CGTexturedQuad();

	GLuint getTexture() {

		return texture;
	}

	void render(const CoreStructures::GUMatrix4& T);
};
