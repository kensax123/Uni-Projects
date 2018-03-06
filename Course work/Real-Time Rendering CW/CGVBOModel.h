
// CGVBOModel version 1.0

#pragma once

// Class to encapsulate a CGModel via composition and extract relevant data to form VBOs

// Note: For version 1.0 we assume models contain a single CGPolyMesh

/*

The attribute location assignments for each VBO in the CGVBOModel class are...

Vertex attribute (VBO)		packet attribute location
----------------------		-------------------------
position					0
colour						1
normal						2

*/

#include <glew\glew.h>
#include <glew\wglew.h>
#include <CoreStructures\CoreStructures.h>

class CGModel;

class CGVBOModel : public CoreStructures::GUObject {


private:

	CGModel			*model;

	GLuint			modelVAO;

	GLuint			meshPositionVBO;
	GLuint			meshColourVBO;
	GLuint			meshNormalVBO;
	GLuint			meshIndexVBO;

	GLuint			numElements;

public:

	CGVBOModel();
	CGVBOModel(CGModel *actualModel, const CoreStructures::GUVector4& vertexColour, bool invertNormals = false);  // store strong ref to CGModel
	~CGVBOModel();

	void render(); // encapsulate rendering with VBOs

};
