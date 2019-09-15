#include <stdio.h>
#include <string>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

class GLShader
{
public:
  GLShader();

  void CreateFromString(const char *vertextCode, const char *fragmentCode);
  void CreateFromFiles(const char *vertextLocation, const char *fragmentLocation);

  std::string ReadFile(const char* fileLocation);

  GLuint GetShaderID();
  GLuint GetAmbIntensityLocation();
  GLuint GetAmbColourLocation();
  GLuint GetDiffuseIntensityLocation();
  GLuint GetDirLightLocation();
  GLuint GetVColourFracLocation();
  GLuint GetProjectionLocation();
  GLuint GetModelLocation();
  GLuint GetViewLocation();

  void Use();
  void Clear();

  ~GLShader();

private:
  GLuint uniAmbIntesity, uniAmbColour, uniDirLightDiffuseIntensity, 
    uniDirLightDirection, uniProjection, uniModel, uniView, 
    uniVertexColourFrac, uniTextColour, shaderID;

  void Compile(const char* vertexCode, const char *fragmentCode);
  void Add(GLuint theProgram, const char*shaderCode, GLenum shaderType);
};
