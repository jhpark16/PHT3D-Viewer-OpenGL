#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include <GLFW/glfw3.h>

class Camera
{
public:
  Camera();
  Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, 
    GLfloat startMoveSpeed, GLfloat startTurnSpeed);

  void keyControl(char key);
  void mouseControl(GLfloat xChange, GLfloat yChange);

  glm::mat4 calculateViewMatrix();

  ~Camera();

private:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  GLfloat yaw; // left or right
  GLfloat pitch; // up or down
  GLfloat roll; // rotate left or right

  GLfloat moveSpeed;
  GLfloat turnSpeed;

  void update();
};

