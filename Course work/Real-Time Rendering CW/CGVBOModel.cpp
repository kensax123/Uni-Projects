
#include "stdafx.h"
#include "CGVBOModel.h"
#include <CGImport\CGModel\CGModel.h>

using namespace std;
using namespace CoreStructures;

CGVBOModel::CGVBOModel() {

	model = NULL;

	modelVAO = 0;
	meshPositionVBO = meshColourVBO = meshNormalVBO = meshIndexVBO = 0;
};


CGVBOModel::CGVBOModel(CGModel *actualModel, const GUVector4& vertexColour, bool invertNormals) {

	model = actualModel;

	if (model) {

		model->retain(); // store strong ref to CGModel

		CGPolyMesh *mesh = model->getMeshAtIndex(0);

		if (mesh) {

			// extract mesh def
			CGBaseMeshDefStruct mdef;
			mesh->createMeshDef(&mdef);

			// build VAO and VBOs

			// setup and bind VAO
			glGenVertexArrays(1, &modelVAO);
			glBindVertexArray(modelVAO);

			// 1. setup VBO for vertex positions (bind to attribute location 0) type: vec4
			glGenBuffers(1, &meshPositionVBO);
			glBindBuffer(GL_ARRAY_BUFFER, meshPositionVBO);
			glBufferData(GL_ARRAY_BUFFER, mdef.N * sizeof(GUVector4), mdef.V, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

			glEnableVertexAttribArray(0);


			// 2. setup VBO for vertex normals (bind to attribute location 2) type: vec3

			// invert normals if specified
			if (invertNormals) {

				for (int i = 0; i<mdef.N; i++)
					mdef.Vn[i] = -(mdef.Vn[i]);
			}

			glGenBuffers(1, &meshNormalVBO);
			glBindBuffer(GL_ARRAY_BUFFER, meshNormalVBO);
			glBufferData(GL_ARRAY_BUFFER, mdef.N * sizeof(GUVector4), mdef.Vn, GL_STATIC_DRAW);

			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

			glEnableVertexAttribArray(2);


			// 3. setup VBO for vertex colour array (local - bind to attribute location 1) type:vec4
			GUVector4 *colourArray = (GUVector4*)malloc(mdef.N * sizeof(GUVector4));

			if (colourArray) {

				for (int i = 0; i<mdef.N; i++)
					colourArray[i] = vertexColour;

				glGenBuffers(1, &meshColourVBO);
				glBindBuffer(GL_ARRAY_BUFFER, meshColourVBO);
				glBufferData(GL_ARRAY_BUFFER, mdef.N * sizeof(GUVector4), colourArray, GL_STATIC_DRAW);

				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

				glEnableVertexAttribArray(1);

				free(colourArray); // dispose of unwanted local resources
			}


			// 4. setup VBO for index array (based on Fv) topology: GL_TRIANGLES
			glGenBuffers(1, &meshIndexVBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mdef.n * sizeof(CGFaceVertex), mdef.Fv, GL_STATIC_DRAW);

			// unbind VAO
			glBindVertexArray(0);

			// store number of actual vertices to render (equals number of faces/triangles * 3 vertices per triangle)
			numElements = mdef.n * 3;
		}

	}
}


CGVBOModel::~CGVBOModel() {

	if (model)
		model->release();

	// dispose of OpenGL resources

	// delete VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &meshPositionVBO);
	glDeleteBuffers(1, &meshNormalVBO);
	glDeleteBuffers(1, &meshColourVBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &meshIndexVBO);

	// delete VAO
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &modelVAO);
}


void CGVBOModel::render() {

	glBindVertexArray(modelVAO);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, (const GLvoid*)0);

	glBindVertexArray(0);
}
