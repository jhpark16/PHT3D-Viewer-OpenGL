#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class GLLight
{
public:
  GLLight();
  GLLight(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntensity, GLfloat xDir, GLfloat yDir,
    GLfloat zDir, GLfloat diffIntensity);
  ~GLLight();

  void Use(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
    GLuint diffuseIntensityLocation, GLuint directionLocation);

protected:
  glm::vec3 colour;
  GLfloat ambientIntensity;
  glm::vec3 direction;
  GLfloat diffuseIntensity;
};

