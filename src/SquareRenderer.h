#pragma once

#include "RenderableObject.h"

class SquareRenderer : public RenderableObject {
public:
  SquareRenderer();
  ~SquareRenderer() override;

  bool Initialize(Shader *shader) override;
  void Render(const glm::mat4 &projectionMatrix) override;
  void Cleanup() override;

  void SetPosition(float x, float y) override;
  void SetSize(float size) override;
  void SetColor(float r, float g, float b, float a = 1.0f) override;
};