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

  GLuint GetProjectionLocation();
  GLuint GetModelLocation();
  GLuint GetViewLocation();
  GLuint GetAmbientIntensityLocation();
  GLuint GetAmbientColourLocation();
  GLuint GetDiffuseIntensityLocation();
  GLuint GetDirectionLocation();
  GLuint GetVColourFracLocation();
  GLuint GetShaderID();

  void Use();
  void Clear();

  ~GLShader();

private:
  GLuint shaderID, uniProjection, uniModel, uniView,
    uniAmbIntesity, uniAmbColour, uniDirLightDiffuseIntensity, 
    uniDirLightDirection, uniVertexColourFrac;

  void Compile(const char* vertexCode, const char *fragmentCode);
  void Add(GLuint theProgram, const char*shaderCode, GLenum shaderType);
};
