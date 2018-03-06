
// TextureLoader.h - Setup OpenGL textures from images loaded using Windows Imaging Component (WIC) or the FreeImage library


#pragma once

#include <glew\glew.h>
#include <string>


// Structure to define properties for new textures
struct TextureGenProperties {

	GLint		internalFormat = GL_RGBA8;
	GLint		minFilter = GL_LINEAR;
	GLint		maxFilter = GL_LINEAR;
	GLfloat		anisotropicLevel = 0.0f;
	GLint		wrap_s = GL_REPEAT;
	GLint		wrap_t = GL_REPEAT;
	bool		genMipMaps = FALSE;

	TextureGenProperties(GLint _format) {

		internalFormat = _format;
	}

	TextureGenProperties(GLint _format, GLint _minf, GLint _maxf, GLfloat _af, GLint _ws, GLint _wt, bool _genmm)
		: internalFormat(_format),
		minFilter(_minf),
		maxFilter(_maxf),
		anisotropicLevel(_af),
		wrap_s(_ws),
		wrap_t(_wt),
		genMipMaps(_genmm) {}

};


enum CGMipmapGenMode { CG_NO_MIPMAP_GEN = 0, CG_CORE_MIPMAP_GEN, CG_EXT_MIPMAP_GEN };


// Note: Comment this out to remove WIC features from texture_loader
#define __CG_USE_WINDOWS_IMAGING_COMPONENT___		1


class TextureLoader {

public:

#ifdef __CG_USE_WINDOWS_IMAGING_COMPONENT___

	// Windows Imaging Component (WIC) based image loader
	static GLuint wicLoadTexture(const std::wstring& textureFilePath);
	static GLuint wicLoadTexture(const std::wstring& textureFilePath, const TextureGenProperties& textureProperties);

#endif


	// FreeImage-based image loader
	static GLuint fiLoadTexture(const std::wstring& textureFilePath);
	static GLuint fiLoadTexture(const std::wstring& textureFilePath, const TextureGenProperties& textureProperties);


	// loadCubeMapTexture creates and returns a cubemap texture in OpenGL.  This uses the WIC library.  Six images are loaded - the path to each of the six images is determined by <directory><file_prefix><face><extension>.  The 'directory' parameter represents the path to the containing folder of the texture files in the form "C:\...\Textures\" for example.  The 'file_prefix' parameter represents the first part of the texture filename.  The <face> part of the filename is created within loadCubeMapTexture.  This is of the form "_positive_x" and "_negative_x" for example.  The extension parameter is of the form ".bmp" for example.  All image filenames must be of the same type and follow the same structure.  For example, given the texture files "snow_positive_x.bmp" and "snow_negative_x.bmp", 'file_prefix' would be "snow" and 'extension' would be ".bmp"

	static GLuint loadCubeMapTexture(
		const std::wstring& directory,
		const std::wstring& file_prefix,
		const std::wstring& file_extension,
		GLenum format,
		GLint minFilter,
		GLint maxFilter,
		const GLfloat anisotropicLevel,
		const GLint wrap_s,
		const GLint wrap_t,
		const GLint wrap_r,
		const bool genMipMaps);
};
