#pragma once

#include <GLFW/glfw3.h>
#include <memory>

class LuaEngine;

class Application {
public:
  Application();
  ~Application();

  bool Initialize();
  void Run();
  void Shutdown();

private:
  void InitializeGLFW();
  void InitializeImGui();
  void Update();
  void Render();

  static void ErrorCallback(int error, const char *description);

  GLFWwindow *m_window;
  std::unique_ptr<LuaEngine> m_luaEngine;
  bool m_isRunning;

  // Window settings
  static constexpr int WINDOW_WIDTH = 800;
  static constexpr int WINDOW_HEIGHT = 600;
};