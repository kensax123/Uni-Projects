
#pragma once

#include <glew\glew.h>
#include <glew\wglew.h>
#include <CoreStructures\GUObject.h>
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\CGColorRGB.h>
#include <string>


class CGFont : public CoreStructures::GUObject {

	GLuint				fnt_base;

public:

	CGFont(const std::wstring& fontName, const int fontSize);
	~CGFont();

	void renderText(const float x, const float y, const CoreStructures::GUMatrix4& view, const CoreStructures::CGColorRGB& colour, const char *formatString, ...) const;

private:

	void deleteFont();

};

