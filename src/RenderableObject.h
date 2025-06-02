#pragma once

#include "Shader.h"
#include <glm.hpp>

class RenderableObject {
public:
  virtual ~RenderableObject() = default;

  virtual bool
  Initialize(Shader *shader) = 0; // Pass a shared or specific shader
  virtual void Render(const glm::mat4 &projectionMatrix) = 0;
  virtual void Cleanup() = 0;

  virtual void SetPosition(float x, float y) = 0;
  virtual void SetSize(float size) = 0; // Or SetDimensions(w,h)
  virtual void SetColor(float r, float g, float b, float a = 1.0f) = 0;

  virtual glm::vec2 GetPosition() const { return m_position; }
  virtual float GetSize() const { return m_size; } // Or GetDimensions()
  virtual glm::vec4 GetColor() const { return m_color; }

protected:
  glm::vec2 m_position = {0.0f, 0.0f};
  float m_size = 1.0f; // For a square, size could be side length
  glm::vec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f}; // Default white
  Shader *m_shader = nullptr;                   // Pointer to the shader to use

  GLuint m_VAO = 0;
  GLuint m_VBO = 0;
  // GLuint m_EBO = 0; // If using element buffer objects
};