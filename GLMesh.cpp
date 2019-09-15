// GLMesh.cpp : A simple mesh management class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: May 2017
// Description: This mesh class sends vertices and indices to GPU and
//   render the geometry.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLMesh.h"

// Setup OpenGL buffers and transfer vertices and indices to GPU buffers
void GLMesh::Create(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, 
  unsigned int numOfIndices, int attLengths[])
{
  int idx{ 0 }; // Current starting location
  int totalSize{ 0 };
  for (int i = 0; i < 3; i++) { totalSize += attLengths[i]; }
  // Update the number of indices
  nIndex = numOfIndices;
  // Generate vertex array names
  glGenVertexArrays(1, &VAO); 
  // Bind a vertex array object
  glBindVertexArray(VAO);
  // Generate index array names
  glGenBuffers(1, &IBO); 
  // Bind IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); 
  // Send the indices to GPU
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*numOfIndices, indices, GL_STATIC_DRAW);
  // Generate vertex buffer names
  glGenBuffers(1, &VBO);
  // Bind VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Send the vertices to GPU
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*numOfVertices, vertices, GL_STATIC_DRAW);
  // Setup Vertex attributes
  glVertexAttribPointer(0, attLengths[0], GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * totalSize, 0);
  glEnableVertexAttribArray(0);
  idx += attLengths[0];
  glVertexAttribPointer(1, attLengths[1], GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * totalSize,
    (void*)(sizeof(vertices[0]) * idx));
  glEnableVertexAttribArray(1);
  idx += attLengths[1];
  glVertexAttribPointer(2, attLengths[2], GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * totalSize,
    (void*)(sizeof(vertices[0]) * idx));
  glEnableVertexAttribArray(2);
  // Unbind VBO (This should be done before unbinding IBO)
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Unbind VAO
  glBindVertexArray(0);
  // Unbind IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLMesh::Render()
{
  // Bind VAO
  glBindVertexArray(VAO);
  // Bind IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  // Draw polygons (triangles)
  glDrawElements(GL_TRIANGLES, nIndex, GL_UNSIGNED_INT, 0);
  // Unbind IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  // Unbind VAO
  glBindVertexArray(0);
}

void GLMesh::Clear()
{
  // Unbind VBO (This should be done before unbinding IBO)
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Unbind VAO
  glBindVertexArray(0);
  // Unbind IBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}