#include "stdafx.h"
#include "Shader.h"

Shader::Shader()
{
  shaderID = 0;
  uniformModel = 0;
  uniformProjection = 0;
}

void Shader::CreateFromString(const char *vertexCode, const char *fragmentCode)
{
  CompileShader(vertexCode, fragmentCode);
}


void Shader::CreateFromFiles(const char *vertexLocation, const char *fragmentLocation)
{
  std::string vertexString = ReadFile(vertexLocation);
  std::string fragmentString = ReadFile(fragmentLocation);
  const char *vertexCode = vertexString.c_str();
  const char *fragmentCode = fragmentString.c_str();

  CompileShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char *fileLocation)
{
  std::string content;
  std::ifstream fileStream(fileLocation, std::ios::in);

  if (!fileStream.is_open()) {
    fprintf(stderr, "Failed to read %s! File doesn't exist.", fileLocation);
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

void Shader::AddShader(GLuint theProgram, const char*shaderCode, GLenum shaderType)
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

GLuint Shader::GetProjectionLocation()
{
  return uniformProjection;
}

GLuint Shader::GetModelLocation()
{
  return uniformModel;
}

GLuint Shader::GetViewLocation()
{
  return uniformView;
}

GLuint Shader::GetAmbientColourLocation()
{
  return uniformAmbientColour;
}

GLuint Shader::GetAmbientIntensityLocation()
{
  return uniformAmbientIntesity;
}

GLuint Shader::GetDiffuseIntensityLocation()
{
  return uniformDiffuseIntensity;
}

GLuint Shader::GetDirectionLocation()
{
  return uniformDirection;
}

GLuint Shader::GetVColourFracLocation()
{
  return uniformVColourFrac;
}

GLuint Shader::GetShaderID()
{
  return shaderID;
}

void Shader::CompileShader(const char* vShader, const char *fShader)
{
  shaderID = glCreateProgram();

  if (!shaderID) {
    fprintf(stderr, "Error creating shader program!\n");
    return;
  }

  AddShader(shaderID, vShader, GL_VERTEX_SHADER);
  AddShader(shaderID, fShader, GL_FRAGMENT_SHADER);

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

  uniformProjection = glGetUniformLocation(shaderID, "projection");
  uniformModel = glGetUniformLocation(shaderID, "model");
  uniformView = glGetUniformLocation(shaderID, "view");
  uniformAmbientColour = glGetUniformLocation(shaderID, "dirLight.colour");
  uniformAmbientIntesity = glGetUniformLocation(shaderID, "dirLight.ambient");
  uniformDirection = glGetUniformLocation(shaderID, "dirLight.direction");
  uniformDiffuseIntensity = glGetUniformLocation(shaderID, "dirLight.diffuse");
  uniformVColourFrac = glGetUniformLocation(shaderID, "vColourFrac");
}

void Shader::UseShader()
{
  glUseProgram(shaderID);
}

void Shader::ClearShader()
{
  if (shaderID != 0) {
    glDeleteProgram(shaderID);
    shaderID = 0;
  }
}

Shader::~Shader()
{

}