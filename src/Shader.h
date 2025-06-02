#pragma once

#include <glad/glad.h>
#include <glm.hpp>
#include <string>

class Shader {
public:
  GLuint ID = 0;

  Shader() = default;
  Shader(const char *vertexPath, const char *fragmentPath,
         bool isSourceCode = false);

  // Activates the shader program
  void Use() const;

  // Utility uniform functions
  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetVec2(const std::string &name, float x, float y) const;
  void SetVec2(const std::string &name, const glm::vec2 &value) const;
  void SetVec3(const std::string &name, float x, float y, float z) const;
  void SetVec3(const std::string &name, const glm::vec3 &value) const;
  void SetVec4(const std::string &name, float x, float y, float z,
               float w) const;
  void SetVec4(const std::string &name, const glm::vec4 &value) const;
  void SetMat4(const std::string &name, const glm::mat4 &mat) const;

  void Cleanup(); // To delete the shader program

private:
  // Utility function for checking shader compilation/linking errors.
  static void CheckCompileErrors(GLuint shader, std::string type);
  static GLuint CompileShader(GLenum shaderType, const char *source,
                              const std::string &typeName);
  static GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
};