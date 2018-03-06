
#pragma once

#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <glew\glew.h>

class ShaderLoader;
class TextureLoader;


class CGSkybox : public CoreStructures::GUObject {

private:

	GLuint					skyboxVAO;

	GLuint					skyboxVertexBuffer;
	GLuint					skyboxNormalVectorBuffer;

	GLuint					skyboxShader;

	GLuint					texture;

public:

	CGSkybox(const std::wstring& cubemapPath, const std::wstring& cubemapName, const std::wstring& cubemapExtension);

	~CGSkybox();

	GLuint skyboxTexture();
	void setSkyboxTexture(const GLuint newTexture);

	void render(const CoreStructures::GUMatrix4& T);
};
