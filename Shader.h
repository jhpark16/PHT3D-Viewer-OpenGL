#include <stdio.h>
#include <string>
#include <fstream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
//#include <GLFW/glfw3.h>

class Shader
{
public:
  Shader();

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

  void UseShader();
  void ClearShader();

  ~Shader();

private:
//  GLuint shader;
  GLuint shaderID, uniformProjection, uniformModel, uniformView,
    uniformAmbientIntesity, uniformAmbientColour, uniformDiffuseIntensity, 
    uniformDirection, uniformVColourFrac;

  void CompileShader(const char* vertexCode, const char *fragmentCode);
  void AddShader(GLuint theProgram, const char*shaderCode, GLenum shaderType);
};
