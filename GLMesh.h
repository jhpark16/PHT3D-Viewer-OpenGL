// GLMesh.h : A simple 
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2017
// Description: 
//
/////////////////////////////////////////////////////////////////////////////

// Include GLEW
#include <GL/glew.h>

class GLMesh 
{
public:
  // Setup OpenGL using vertices and indices 
  void Create(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, 
    unsigned int numOfIndices, int attLengths[]);
  // Render the polygones
  void Render();
  // Clear the buffer
  void Clear();
private:
  // Vertex Array Object(VAO), vertex buffer object(VBO), 
  // Index Buffer Object(IBO), number of indices
  GLuint VAO, VBO, IBO, nIndex;
};