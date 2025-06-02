#include "Application.h"
#include "LuaEngine.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

Application::Application()
    : m_window(nullptr), m_luaEngine(nullptr), m_isRunning(false) {}

Application::~Application() { Shutdown(); }

bool Application::Initialize() {
  try {
    InitializeGLFW();
    InitializeImGui();

    m_luaEngine = std::make_unique<LuaEngine>();
    if (!m_luaEngine->Initialize()) {
      std::cerr << "Failed to initialize Lua engine\n";
      return false;
    }

    m_isRunning = true;
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Initialization failed: " << e.what() << "\n";
    return false;
  }
}

void Application::InitializeGLFW() {
  glfwSetErrorCallback(ErrorCallback);

  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                              "Minimal ImGui + Lua App", nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1); // VSync

  // Set up keyboard callback for live reload
  glfwSetWindowUserPointer(m_window, this);
  glfwSetKeyCallback(m_window, KeyCallback);
}

void Application::InitializeImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void Application::Run() {
  while (m_isRunning && !glfwWindowShouldClose(m_window)) {
    glfwPollEvents();
    Update();
    Render();
  }
}

void Application::Update() {
  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Execute Lua GUI
  m_luaEngine->DrawGUI();
}

void Application::Render() {
  ImGui::Render();

  int width;
  int height;
  glfwGetFramebufferSize(m_window, &width, &height);
  glViewport(0, 0, width, height);

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(m_window);
}

void Application::Shutdown() {
  m_luaEngine.reset();

  if (ImGui::GetCurrentContext() != nullptr) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  if (m_window != nullptr) {
    glfwDestroyWindow(m_window);
    m_window = nullptr;
  }

  glfwTerminate();
}

void Application::ErrorCallback(int error, const char *description) {
  std::cerr << "GLFW Error (" << error << "): " << description << "\n";
}

void Application::KeyCallback(GLFWwindow *window, int key, int scancode,
                              int action, int mods) {
  auto *app = static_cast<Application *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    // F5 for force reload
    if (key == GLFW_KEY_F5) {
      app->m_luaEngine->ForceReload();
    }
    // Ctrl+R for force reload
    else if (key == GLFW_KEY_R && (mods & GLFW_MOD_CONTROL)) {
      app->m_luaEngine->ForceReload();
    }
    // F6 to toggle auto-reload
    else if (key == GLFW_KEY_F6) {
      bool currentState = app->m_luaEngine->IsAutoReloadEnabled();
      app->m_luaEngine->SetAutoReload(!currentState);
      std::cout << "Auto-reload " << (!currentState ? "enabled" : "disabled")
                << "\n";
    }
  }
}