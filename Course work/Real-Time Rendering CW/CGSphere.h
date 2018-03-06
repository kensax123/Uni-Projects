
// CGSphere version 1.0

#pragma once

// Class to encapsulate a procedural sphere model stored in a VBO

/*

The attribute location assignments for each VBO in the CGSphere class are...

Vertex attribute (VBO)		packet attribute location
----------------------		-------------------------
position					0
(diffuse) colour			1
normal						2
texture coord 0				3

*/

#include <CoreStructures\GUMemory.h>
#include <CoreStructures\GUObject.h>
#include <glew\glew.h>
#include <glew\wglew.h>
#include <CoreStructures\GUVector4.h>

// Model a sphere procedurally. The z coordinates are determined by the handedness specified in the constructor parameter.  The z coordinate is inverted and face ordering reversed for the right-handed system (OpenGL).  The default is the left-handed system (DirectX)

enum CGSphereHandedness { CG_LEFTHANDED, CG_RIGHTHANDED };

class CGSphere : public CoreStructures::GUObject {

private:

	GLuint			sphereVAO;

	GLuint			meshPositionVBO;
	GLuint			meshColourVBO;
	GLuint			meshNormalVBO;
	GLuint			meshTexCoordVBO;
	GLuint			meshIndexVBO;

	GLuint			numElements;

public:

	CGSphere();
	CGSphere(int n, int m, float radius, const CoreStructures::GUVector4& vertexColour, const CGSphereHandedness orientation = CG_LEFTHANDED);  // n = slices, m = stacks
	~CGSphere();

	void render(); // encapsulate rendering with VBOs

};
