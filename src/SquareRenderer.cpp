#include "SquareRenderer.h"
#include <glad/glad.h>
#include <iostream>

SquareRenderer::SquareRenderer() {
  // Default values are set in RenderableObject
}

SquareRenderer::~SquareRenderer() {
  Cleanup(); // Ensure cleanup if not explicitly called
}

bool SquareRenderer::Initialize(Shader *shader) {
  if ((shader == nullptr) || shader->ID == 0) {
    std::cerr << "SquareRenderer::Initialize: Invalid shader provided."
              << std::endl;
    return false;
  }
  m_shader = shader;

  // Normalized square vertices (origin at top-left for this example)
  // These will be scaled and translated by uniforms.
  float vertices[] = {
      // positions (will be multiplied by size and translated by position in
      // shader)
      0.0f, 0.0f, // Top-left
      1.0f, 0.0f, // Top-right
      1.0f, 1.0f, // Bottom-right
      0.0f, 1.0f  // Bottom-left
  };

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return m_VAO != 0 && m_VBO != 0;
}

void SquareRenderer::Render(const glm::mat4 &projectionMatrix) {
  if ((m_shader == nullptr) || m_shader->ID == 0 || m_VAO == 0) {
    return; // Not initialized or invalid shader
  }

  m_shader->Use();
  m_shader->SetMat4("projection", projectionMatrix);
  m_shader->SetVec2("u_position", m_position);
  m_shader->SetFloat("u_size", m_size);
  m_shader->SetVec4("u_color", m_color); // Pass RGBA

  glBindVertexArray(m_VAO);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Draw quad as triangle fan
  glBindVertexArray(0);
  // glUseProgram(0); // Don't unbind shader here if multiple objects use the
  // same one sequentially
}

void SquareRenderer::Cleanup() {
  if (m_VAO != 0) {
    glDeleteVertexArrays(1, &m_VAO);
    m_VAO = 0;
  }
  if (m_VBO != 0) {
    glDeleteBuffers(1, &m_VBO);
    m_VBO = 0;
  }
  // Note: The shader is owned by the Application (or a resource manager)
  // so this class should not delete it.
  m_shader = nullptr;
}

void SquareRenderer::SetPosition(float x, float y) {
  m_position.x = x;
  m_position.y = y;
}

void SquareRenderer::SetSize(float size) { m_size = size > 0 ? size : 1.0f; }

void SquareRenderer::SetColor(float r, float g, float b, float a) {
  m_color = glm::vec4(r, g, b, a);
}