#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Constructor that reads from files (default) or directly from source strings
Shader::Shader(const char *vertexPathOrSource, const char *fragmentPathOrSource,
               bool isSourceCode) {
  std::string vertexCode;
  std::string fragmentCode;

  if (isSourceCode) {
    vertexCode = vertexPathOrSource;
    fragmentCode = fragmentPathOrSource;
  } else {
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      // Open files
      vShaderFile.open(vertexPathOrSource);
      fShaderFile.open(fragmentPathOrSource);
      std::stringstream vShaderStream;
      std::stringstream fShaderStream;
      // Read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // Close file handlers
      vShaderFile.close();
      fShaderFile.close();
      // Convert stream into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
      std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
                << std::endl;
      ID = 0;
      return;
    }
  }

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // 2. Compile shaders
  GLuint vertex;
  GLuint fragment;
  vertex = CompileShader(GL_VERTEX_SHADER, vShaderCode, "VERTEX");
  if (vertex == 0u) {
    ID = 0;
    return;
  }

  fragment = CompileShader(GL_FRAGMENT_SHADER, fShaderCode, "FRAGMENT");
  if (fragment == 0u) {
    glDeleteShader(vertex);
    ID = 0;
    return;
  }

  // Shader Program
  ID = LinkProgram(vertex, fragment);

  // Delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

GLuint Shader::CompileShader(GLenum shaderType, const char *source,
                             const std::string &typeName) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);
  CheckCompileErrors(shader, typeName);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  return (success != 0) ? shader : 0;
}

GLuint Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);
  CheckCompileErrors(program, "PROGRAM");
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  return (success != 0) ? program : 0;
}

void Shader::Use() const {
  if (ID != 0) {
    glUseProgram(ID);
  } else {
    // std::cerr << "Trying to use an invalid shader program (ID=0)" <<
    // std::endl;
  }
}

void Shader::Cleanup() {
  if (ID != 0) {
    glDeleteProgram(ID);
    ID = 0;
  }
}

void Shader::CheckCompileErrors(GLuint shader, std::string type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr
          << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success == 0) {
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cerr
          << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  }
}

// Utility uniform functions implementations
void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::SetVec2(const std::string &name, float x, float y) const {
  glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::SetVec2(const std::string &name, const glm::vec2 &value) const {
  glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec3(const std::string &name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec4(const std::string &name, float x, float y, float z,
                     float w) const {
  glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}
void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     &mat[0][0]);
}