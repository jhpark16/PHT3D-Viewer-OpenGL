// GLLight.cpp : Light class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: August 2017
// Description: Light class containing ambient and directional light.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLLight.h"

GLLight::GLLight():
  GLLight(1.f, 1.f, 1.f, 1.f, 0.f, -1.f, 0.f, 0.f)
{
  
}

GLLight::GLLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity, GLfloat xDir, GLfloat yDir,
  GLfloat zDir, GLfloat diffIntensity):
  colour { glm::vec3(red, green, blue) }, ambientIntensity {ambIntensity}, 
  direction{ glm::vec3(xDir, yDir, zDir) }, diffuseIntensity { diffIntensity }
{
  
}

void GLLight::Use(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
  GLuint diffuseIntensityLocation, GLuint directionLocation)
{
  glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
  glUniform1f(ambientIntensityLocation, ambientIntensity);

  glUniform3f(directionLocation, direction.x, direction.y, direction.z);
  glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

GLLight::~GLLight()
{
}
