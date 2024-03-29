// GLCamera.cpp : Camera class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: August 2017
// Description: A simple camera simulating the main viewer's position and direction
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLCamera.h"

GLCamera::GLCamera()
{

}

GLCamera::GLCamera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch)
{
  position = startPosition;
  worldUp = startUp;
  yaw = startYaw;
  pitch = startPitch;
  front = glm::vec3(0.f, 0.f, -1.f);

  update();
}

glm::mat4 GLCamera::calculateViewMatrix()
{
  return glm::lookAt(position, position + front, up);
}

void GLCamera::update()
{
  front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
  front = glm::normalize(front);

  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

GLCamera::~GLCamera()
{
}
