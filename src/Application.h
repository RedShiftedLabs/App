#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <memory>

class RenderableObject;
class SquareRenderer;
class LuaEngine;

class Application {
public:
  Application();
  ~Application();

  bool Initialize();
  void Run();
  void Shutdown();

  void SetShapePosition(float x, float y);
  void SetShapeSize(float size);
  void SetShapeColor(float r, float g, float b, float a = 1.0f);
  void SetBackgroundColor(float r, float g, float b, float a);

  static glm::vec2 getWindowDimensions();

  glm::vec2 GetShapePositionLua() const;
  float GetShapeSizeLua() const;
  glm::vec4 GetShapeColorLua() const;

private:
  void InitializeGLFW();
  void InitializeImGui();
  void InitializeRenderables();
  void Update();
  void Render();
  void RenderScene();
  void CleanupRenderables();

  // --- FPS Calculation Members ---
  void UpdateWindowTitleWithFPS();
  double m_lastTime = 0.0;
  int m_frameCount = 0;
  double m_fpsTimeAccumulator = 0.0;
  int m_fpsFrameCountAccumulator = 0;
  static constexpr double FPS_UPDATE_INTERVAL =
      0.5; // Update FPS in title every 0.5 seconds

  static void ErrorCallback(int error, const char *description);
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

  void HandleMouseInput();
  GLFWwindow *m_window;
  std::string m_baseWindowTitle = "Minimal ImGui + Lua App";
  std::unique_ptr<LuaEngine> m_luaEngine;
  bool m_isRunning;

  float m_backgroundColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};

  Shader m_simpleShapeShader;                   // Shader object
  std::unique_ptr<SquareRenderer> m_mainSquare; // Example renderable
  // std::vector<std::unique_ptr<RenderableObject>> m_renderables; // For
  // multiple objects

  glm::mat4 m_projectionMatrix;

  bool m_isDraggingShape = false;
  glm::vec2 m_dragOffset = {0.0f, 0.0f};

  // Window settings
  static constexpr int WINDOW_WIDTH = 1080 * 1.25;
  static constexpr int WINDOW_HEIGHT = 720;

  static const char *s_vertexShaderSource;
  static const char *s_fragmentShaderSource;
};