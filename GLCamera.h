#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GLCamera
{
public:
  GLCamera();
  GLCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch);

  glm::mat4 calculateViewMatrix();

  ~GLCamera();

private:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  GLfloat yaw; // left or right
  GLfloat pitch; // up or down
  GLfloat roll; // rotate left or right

  void update();
};

