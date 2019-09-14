// Include GLEW
#include <GL/glew.h>

// Include GLFW
//#include <GLFW/glfw3.h>

class Mesh 
{
public:
  void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
  void RenderMesh();
  void ClearMesh();
private:
  GLuint VAO, VBO, IBO, indexCount;
};