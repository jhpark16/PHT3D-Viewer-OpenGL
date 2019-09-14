// Requires OpenGL 3.3
#version 330

layout (location = 0) in vec3 position; // the position of the vertex
layout (location = 1) in vec3 vColour;
layout (location = 2) in vec3 normal; // Normal vector for the vertex


uniform mat4 projection;    
uniform mat4 view;                                      
uniform mat4 model;

out vec4 vertexColour; // Vertex colour
out vec3 Normal;

void main()
{
  // Transform vertex location using projection, view and model matrices
  gl_Position = projection * view * model * vec4(position, 1.0);
  //gl_Position = vec4(0,0,0, 1.0);
  Normal = mat3(transpose(inverse(model))) * normal;
  vertexColour = vec4(vColour, 1.0f);
  //vertexColour = vec4(0,1,0, 1.0f);
  //vertexColour = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}
