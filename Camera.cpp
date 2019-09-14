#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{

}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, 
  GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
  position = startPosition;
  worldUp = startUp;
  yaw = startYaw;
  pitch = startPitch;
  front = glm::vec3(0.f, 0.f, -1.f);

  moveSpeed = startMoveSpeed;
  turnSpeed = startTurnSpeed;

  update();
}

void Camera::keyControl(char key)
{
  if (key == 'W') {
    position += front * moveSpeed;
  }
  if (key == 'S') {
    position -= front * moveSpeed;
  }
  if (key == 'A') {
    position -= right * moveSpeed;
  }
  if (key == 'D') {
    position += right * moveSpeed;
  }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
  xChange *= turnSpeed;
  yChange *= turnSpeed;

  yaw += xChange;
  pitch += yChange;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  }
  if (pitch < -89.0f) {
    pitch = -89.0f;
  }
  update();
}

glm::mat4 Camera::calculateViewMatrix()
{
  return glm::lookAt(position, position + front, up);
}

void Camera::update()
{
  front.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
  front = glm::normalize(front);

  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera()
{
}
