// GLShader.cpp : Shader managing class
//
// Author: Jungho Park (jhpark16@gmail.com)
// Date: August 2017
// Description: A simple shader managing class, the shader uses ambient and directional light.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLShader.h"

GLShader::GLShader() :uniAmbIntesity{ 0 }, uniAmbColour{ 0 }, uniDirLightDiffuseIntensity{ 0 },
uniDirLightDirection{ 0 }, uniProjection{ 0 }, uniModel{ 0 }, uniView{ 0 },
uniVertexColourFrac{ 0 }, uniTextColour{}, shaderID{ 0 }
{
}

// Create shaders using the string values provided
void GLShader::CreateFromString(const char *vertexCode, const char *fragmentCode)
{
  Compile(vertexCode, fragmentCode);
}

// Read shaders from files and compile the shaders
void GLShader::CreateFromFiles(const char *vertexLocation, const char *fragmentLocation)
{
  std::string vertexString = ReadFile(vertexLocation);
  std::string fragmentString = ReadFile(fragmentLocation);
  const char *vertexCode = vertexString.c_str();
  const char *fragmentCode = fragmentString.c_str();

  Compile(vertexCode, fragmentCode);
}

// Read a shader file
std::string GLShader::ReadFile(const char *fileLocation)
{
  std::string content;
  std::ifstream fileStream(fileLocation, std::ios::in);

  if (!fileStream.is_open()) {
    CString tstr = "Failed to read ";
    tstr += fileLocation;
    ::MessageBox(NULL,tstr.GetString(),"File Open Error",MB_OK);
    std::cout << "Failed to read " << fileLocation << "!" << std::endl;
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

// Add shader code to the class
void GLShader::Add(GLuint theProgram, const char*shaderCode, GLenum shaderType)
{
  GLuint theShader = glCreateShader(shaderType);
  const GLchar* theCode[1];
  theCode[0] = shaderCode;
  GLint codeLength[1];
  codeLength[0] = strlen(shaderCode);

  // Add the source code to the shader
  glShaderSource(theShader, 1, theCode, codeLength);
  // Compile the shader
  glCompileShader(theShader);

  GLint result = 0;
  GLchar eLog[1024] = { 0 };

  // Check error
  glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

  if (!result) {
    glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
    std::cout << "Error compiling the " << (shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") 
      << "shader: " << eLog << std::endl;
    return;
  }
  // Attach the shader
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

GLuint GLShader::GetAmbColourLocation()
{
  return uniAmbColour;
}

GLuint GLShader::GetAmbIntensityLocation()
{
  return uniAmbIntesity;
}

GLuint GLShader::GetDiffuseIntensityLocation()
{
  return uniDirLightDiffuseIntensity;
}

GLuint GLShader::GetDirLightLocation()
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
    std::cout << "Error creating shader program!\n";
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
    std::cout << "Error linking program: " << eLog << std::endl;
    return;
  }

  glValidateProgram(shaderID);
  glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
  if (!result) {
    glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
    std::cout << "Error validating program: " << eLog << std::endl;
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
  uniTextColour = glGetUniformLocation(shaderID, "textColor");
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