#include "stdafx.h"
#include "Light.h"

Light::Light():
  Light(1.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f, 0.f)
{
  
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat xDir, GLfloat yDir,
  GLfloat zDir, GLfloat dIntensity):
  colour { glm::vec3(red, green, blue) }, ambientIntensity {aIntensity}, 
  direction{ glm::vec3(xDir, yDir, zDir) }, diffuseIntensity { dIntensity }
{
  
}

void Light::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
  GLuint diffuseIntensityLocation, GLuint directionLocation)
{
  glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
  glUniform1f(ambientIntensityLocation, ambientIntensity);

  glUniform3f(directionLocation, direction.x, direction.y, direction.z);
  glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

Light::~Light()
{
}
