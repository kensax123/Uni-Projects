
#include "stdafx.h"
#include "CGSphere.h"

using namespace std;
using namespace CoreStructures;

// private structures

// could use CGTextureCoord but this is a more complete 4D vector - here only store uv coords for memory efficiency
struct TexCoord2 {

	float u, v;
};


// again for memory efficieny, only store x, y, z for normals
struct NormalVec3 {

	float x, y, z;
};

CGSphere::CGSphere() {

	sphereVAO = 0;
	meshPositionVBO = meshColourVBO = meshNormalVBO = meshTexCoordVBO = meshIndexVBO = 0;

	numElements = 0;
};


// n = slices, m = stacks
CGSphere::CGSphere(int n, int m, float radius, const GUVector4& vertexColour, const CGSphereHandedness orientation) {

	int n_ = n + 1;
	int m_ = m + 1;

	const float pi2 = gu_pi * 2.0f;
	const float zc = ((orientation == CG_RIGHTHANDED) ? -1.0f : 1.0f);

	GUVector4 *posArray = (GUVector4*)malloc(n_*m_*sizeof(GUVector4));
	GUVector4 *colourArray = (GUVector4*)malloc(n_*m_*sizeof(GUVector4));
	NormalVec3 *normalArray = (NormalVec3 *)malloc(n_*m_*sizeof(NormalVec3));
	TexCoord2 *texArray = (TexCoord2 *)malloc(n_*m_*sizeof(TexCoord2));

	unsigned int *indexArray = (unsigned int *)malloc(n * m * 2 * 3 * sizeof(unsigned int)); // GL_TRIANGLES topology

	if (posArray && colourArray && normalArray && texArray && indexArray) {

		// first create column(stack) major arrays of order (m+1 x n+1) to store relevant data before setting up VBOs

		float theta_i = 0.0f;

		for (int i = 0; i <= n; i++, theta_i += (pi2 / (float)n)) { // column index in array

			float theta_j = 0.0f;

			for (int j = 0; j <= m; j++, theta_j += (gu_pi / (float)m)) { // row index in array

				float x = cosf(theta_i) * sinf(theta_j) * radius;
				float y = cosf(theta_j) * radius;
				float z = zc * sinf(theta_i) * sinf(theta_j) * radius;  // invert z for right-handed coordinate system (using zc coefficient)

				int index = i * m_ + j;

				GUVector4 P = GUVector4(x, y, z, 1.0f);
				posArray[index] = P;

				colourArray[index] = vertexColour;

				GUVector4 N = P;
				N.normalise();
				normalArray[index].x = N.x;
				normalArray[index].y = N.y;
				normalArray[index].z = N.z;

				texArray[index].u = theta_i / pi2;
				texArray[index].v = (theta_j / gu_pi);
			}
		}


		// setup index array
		unsigned int *vIndex = indexArray;

		for (unsigned int i = 0; i<(unsigned int)n; i++) {

			for (unsigned int j = 0; j<(unsigned int)m; j++) {

				unsigned int v1, v2, v3, v4;

				v1 = i * m_ + j;
				v2 = (i + 1) * m_ + j;
				v3 = (i + 1) * m_ + j + 1;
				v4 = i * m_ + j + 1;


				// face ordering is determined by handedness
				if (orientation == CG_RIGHTHANDED) {

					// triangle1 : v1, v4, v2
					vIndex[0] = v1;
					vIndex[1] = v4;
					vIndex[2] = v2;

					// triangle2 : v2, v4, v3
					vIndex[3] = v2;
					vIndex[4] = v4;
					vIndex[5] = v3;

				}
				else {

					// triangle1 : v1, v2, v4
					vIndex[0] = v1;
					vIndex[1] = v2;
					vIndex[2] = v4;

					// triangle2 : v2, v3, v4
					vIndex[3] = v2;
					vIndex[4] = v3;
					vIndex[5] = v4;

				}

				vIndex += 6;
			}
		}



		// setup VAO and VBO for sphere

		// setup and bind VAO
		glGenVertexArrays(1, &sphereVAO);
		glBindVertexArray(sphereVAO);

		// 1. setup VBO for vertex positions (bind to attribute location 0) type: vec4
		glGenBuffers(1, &meshPositionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, meshPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, n_ * m_ * sizeof(GUVector4), posArray, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glEnableVertexAttribArray(0);

		// 2. setup VBO for vertex colour array (local - bind to attribute location 1) type:vec4
		glGenBuffers(1, &meshColourVBO);
		glBindBuffer(GL_ARRAY_BUFFER, meshColourVBO);
		glBufferData(GL_ARRAY_BUFFER, n_ * m_ * sizeof(GUVector4), colourArray, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glEnableVertexAttribArray(1);

		// 3. setup VBO for vertex normals (bind to attribute location 2) type: vec3
		glGenBuffers(1, &meshNormalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, meshNormalVBO);
		glBufferData(GL_ARRAY_BUFFER, n_ * m_ * sizeof(NormalVec3), normalArray, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glEnableVertexAttribArray(2);

		// 4. setup VBO for texture coord (bind to attribute location 3) type: vec2
		glGenBuffers(1, &meshTexCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, meshTexCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, n_ * m_ * sizeof(TexCoord2), texArray, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		glEnableVertexAttribArray(3);


		// 5. setup VBO for index array (based on indexArray) topology: GL_TRIANGLES
		glGenBuffers(1, &meshIndexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * m * 2 * 3 * sizeof(unsigned int), indexArray, GL_STATIC_DRAW);

		// unbind VAO
		glBindVertexArray(0);

		// store number of actual vertices to render (equals number of faces/triangles * 3 vertices per triangle)
		numElements = n * m * 2 * 3;
	}


	// cleanup
	if (posArray)
		free(posArray);

	if (colourArray)
		free(colourArray);

	if (normalArray)
		free(normalArray);

	if (texArray)
		free(texArray);

	if (indexArray)
		free(indexArray);
}


CGSphere::~CGSphere() {
}


void CGSphere::render() {

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, (const GLvoid*)0);

	glBindVertexArray(0);
}
