#include "stdafx.h"
#include "GLShader.h"

GLShader::GLShader()
{
  shaderID = 0;
  uniModel = 0;
  uniProjection = 0;
}

void GLShader::CreateFromString(const char *vertexCode, const char *fragmentCode)
{
  Compile(vertexCode, fragmentCode);
}


void GLShader::CreateFromFiles(const char *vertexLocation, const char *fragmentLocation)
{
  std::string vertexString = ReadFile(vertexLocation);
  std::string fragmentString = ReadFile(fragmentLocation);
  const char *vertexCode = vertexString.c_str();
  const char *fragmentCode = fragmentString.c_str();

  Compile(vertexCode, fragmentCode);
}

std::string GLShader::ReadFile(const char *fileLocation)
{
  std::string content;
  std::ifstream fileStream(fileLocation, std::ios::in);

  if (!fileStream.is_open()) {
    fprintf(stderr, "Failed to read %s!", fileLocation);
    return "";
  }
  std::string line = "";
  while (!fileStream.eof()) {
    std::getline(fileStream, line);
    content.append(line + "\n");
  }
  fileStream.close();
  return content;
}

void GLShader::Add(GLuint theProgram, const char*shaderCode, GLenum shaderType)
{
  GLuint theShader = glCreateShader(shaderType);
  const GLchar* theCode[1];
  theCode[0] = shaderCode;
  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode);

  glShaderSource(theShader, 1, theCode, codeLength);
  glCompileShader(theShader);

  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

  if (!result) {
    glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
    fprintf(stderr, "Error compiling the %s shader: '%s'\n", (shaderType==GL_VERTEX_SHADER?"vertex":"fragment"), eLog);
    return;
  }

  glAttachShader(theProgram, theShader);
}

GLuint GLShader::GetProjectionLocation()
{
  return uniProjection;
}

GLuint GLShader::GetModelLocation()
{
  return uniModel;
}

GLuint GLShader::GetViewLocation()
{
  return uniView;
}

GLuint GLShader::GetAmbientColourLocation()
{
  return uniAmbColour;
}

GLuint GLShader::GetAmbientIntensityLocation()
{
  return uniAmbIntesity;
}

GLuint GLShader::GetDiffuseIntensityLocation()
{
  return uniDirLightDiffuseIntensity;
}

GLuint GLShader::GetDirectionLocation()
{
  return uniDirLightDirection;
}

GLuint GLShader::GetVColourFracLocation()
{
  return uniVertexColourFrac;
}

GLuint GLShader::GetShaderID()
{
  return shaderID;
}

void GLShader::Compile(const char* vShader, const char *fShader)
{
  shaderID = glCreateProgram();

  if (!shaderID) {
    fprintf(stderr, "Error creating shader program!\n");
    return;
  }

  Add(shaderID, vShader, GL_VERTEX_SHADER);
  Add(shaderID, fShader, GL_FRAGMENT_SHADER);

  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  glLinkProgram(shaderID);
  glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
    fprintf(stderr, "Error linking program: '%s'\n", eLog);
    return;
  }

  glValidateProgram(shaderID);
  glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
    fprintf(stderr, "Error validating program: '%s'\n", eLog);
    return;
  }

  uniProjection = glGetUniformLocation(shaderID, "projection");
  uniModel = glGetUniformLocation(shaderID, "model");
  uniView = glGetUniformLocation(shaderID, "view");
  uniAmbColour = glGetUniformLocation(shaderID, "dirLight.colour");
  uniAmbIntesity = glGetUniformLocation(shaderID, "dirLight.ambient");
  uniDirLightDirection = glGetUniformLocation(shaderID, "dirLight.direction");
  uniDirLightDiffuseIntensity = glGetUniformLocation(shaderID, "dirLight.diffuse");
  uniVertexColourFrac = glGetUniformLocation(shaderID, "vColourFrac");
}

void GLShader::Use()
{
  glUseProgram(shaderID);
}

void GLShader::Clear()
{
  if (shaderID != 0) {
    glDeleteProgram(shaderID);
    shaderID = 0;
  }
}

GLShader::~GLShader()
{

}