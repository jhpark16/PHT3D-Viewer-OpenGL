// Requires OpenGL 3.3
#version 330

in vec4 vertexColour; // Vertex colour
in vec3 Normal; // Normal vector

out vec4 colour; // Output colour for the fragment

// Directional Light
struct DirLight
{
  vec3 colour; // Colour of the light
  float ambient; // ambient colour intensity
  vec3 direction; // direction of the DirLight
  float diffuse; // the Intensity of dir light
};

uniform DirLight dirLight;
uniform float vColourFrac;

void main()
{
  vec4 ambient = vec4(dirLight.colour, 1.0f) * dirLight.ambient;
  float diffuseFraction = max(dot(normalize(Normal), normalize(dirLight.direction)), 0.f);
  vec4 diffuse = vec4(dirLight.colour, 1.f) * dirLight.diffuse * diffuseFraction;
  // The final colour is the combination of vertex colour and other light source
  colour = vColourFrac*vertexColour + (1.0-vColourFrac) * vertexColour * (ambient+diffuse); 
  //colour = vertexColour * (ambient); 
  //colour = vec4(1,1,0,1); 
}
