#include "Application.h"
#include "LuaEngine.h"
#include "SquareRenderer.h"
#include <gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

// Define static shader sources
const char *Application::s_vertexShaderSource = R"(
    #version 410 core
    layout (location = 0) in vec2 aPos; 

    uniform mat4 projection;
    uniform vec2 u_position; 
    uniform float u_size;    

    void main() {
        vec2 worldPos = aPos * u_size + u_position;
        gl_Position = projection * vec4(worldPos, 0.0, 1.0);
    }
)";

const char *Application::s_fragmentShaderSource = R"(
    #version 410 core
    out vec4 FragColor;
    uniform vec4 u_color; // Changed to vec4 for RGBA

    void main() {
        FragColor = u_color;
    }
)";

Application::Application()
    : m_window(nullptr), m_luaEngine(nullptr), m_isRunning(false) {}

Application::~Application() { Shutdown(); }

bool Application::Initialize() {
  try {
    InitializeGLFW();
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
      throw std::runtime_error("Failed to initialize GLAD");
    }
    InitializeImGui();
    InitializeRenderables(); // Initialize renderables (shader, square)

    m_luaEngine = std::make_unique<LuaEngine>();
    if (!m_luaEngine->Initialize(this)) {
      std::cerr << "Failed to initialize Lua engine\n";
      CleanupRenderables();
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

  if (glfwInit() == 0) {
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
  if (m_window == nullptr) {
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
  ImGui_ImplOpenGL3_Init("#version 410 core");
}

void Application::InitializeRenderables() {
  // Create and compile the shader
  // Pass true for the third argument as we are providing source code directly
  m_simpleShapeShader =
      Shader(s_vertexShaderSource, s_fragmentShaderSource, true);
  if (m_simpleShapeShader.ID == 0) {
    throw std::runtime_error("Failed to create simple shape shader program");
  }

  // Create and initialize the square
  m_mainSquare = std::make_unique<SquareRenderer>();
  if (!m_mainSquare->Initialize(
          &m_simpleShapeShader)) { // Pass pointer to the shader
    throw std::runtime_error("Failed to initialize main square renderer");
  }
  // Set initial properties (could also be done via Lua after init)
  m_mainSquare->SetPosition(150.0f, 150.0f);
  m_mainSquare->SetSize(50.0f);
  m_mainSquare->SetColor(1.0f, 0.0f, 0.0f, 1.0f); // Red, fully opaque

  // Setup projection matrix (do this once or on window resize)
  float L = 0.0f;
  float R = static_cast<float>(WINDOW_WIDTH);
  float B = static_cast<float>(WINDOW_HEIGHT); // Y positive down
  float T = 0.0f;                              // Y positive down
  // Using GLM for ortho projection
  m_projectionMatrix = glm::ortho(L, R, B, T, -1.0f, 1.0f);
  // If not using GLM, you can use the raw array as before:
  // const float ortho_projection_raw[4][4] = {
  //     { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
  //     { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
  //     { 0.0f,         0.0f,        -1.0f,   0.0f },
  //     { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
  // };
  // m_projectionMatrix = glm::make_mat4(&ortho_projection_raw[0][0]); // if you
  // have raw data
}

void Application::Run() {
  while (m_isRunning && (glfwWindowShouldClose(m_window) == 0)) {
    glfwPollEvents();
    Update();
    Render();
  }
}

void Application::Update() {
  glfwPollEvents();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  HandleMouseInput();
  if (m_luaEngine) {
    m_luaEngine->DrawGUI();
  }
}

void Application::HandleMouseInput() {
  if (!m_mainSquare) {
    return; // No shape to drag
  }

  ImGuiIO &io = ImGui::GetIO();
  glm::vec2 mousePos = {io.MousePos.x, io.MousePos.y};

  // Check if ImGui wants to capture the mouse (e.g., if a window is hovered or
  // active) If our shape rendering is independent of ImGui windows, we might
  // want to drag even if an ImGui window isn't focused, UNLESS an ImGui item is
  // *actively* being interacted with. A simpler check for now: if ImGui is NOT
  // using the mouse, we can.
  bool imguiWantsMouse = io.WantCaptureMouse;

  // Get shape properties (assuming RenderableObject has getters or direct
  // access for bounds checking) For SquareRenderer, we can temporarily access
  // its members or add getters. Let's assume RenderableObject gets methods to
  // retrieve its bounds. For now, we'll directly use m_mainSquare->m_position
  // and m_mainSquare->m_size (needs friendship or public access or getters in
  // RenderableObject/SquareRenderer) Let's modify RenderableObject and
  // SquareRenderer for this. For simplicity here, I'll assume m_mainSquare has
  // accessible m_position and m_size. If these are protected in
  // RenderableObject, SquareRenderer needs public getters. For now, to make it
  // compile, let's assume we added these getters to SquareRenderer.

  // To access m_position and m_size from SquareRenderer, they need to be
  // accessible. Option 1: Make them public in RenderableObject (quickest for
  // now, but less encapsulated) Option 2: Add public getters in
  // RenderableObject and SquareRenderer Let's go with adding getters to
  // RenderableObject and SquareRenderer for better practice. (See Step 2a below
  // for these modifications)

  glm::vec2 shapePos = m_mainSquare->GetPosition();
  float shapeSize = m_mainSquare->GetSize();
  glm::vec4 shapeColor = m_mainSquare->GetColor();

  // Simple AABB (Axis-Aligned Bounding Box) hit test for the square
  bool mouseOverShape =
      (mousePos.x >= shapePos.x && mousePos.x <= shapePos.x + shapeSize &&
       mousePos.y >= shapePos.y && mousePos.y <= shapePos.y + shapeSize);

  if (!m_isDraggingShape) {
    if (mouseOverShape && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !imguiWantsMouse) {
      m_isDraggingShape = true;
      m_dragOffset =
          mousePos - shapePos; // Calculate offset from shape's top-left
      // Optional: Visual feedback for dragging (e.g., change color slightly)
      // m_mainSquare->SetColor(shapeColor.r * 0.8f, shapeColor.g * 0.8f,
      // shapeColor.b * 0.8f, shapeColor.a);
    }
  }

  if (m_isDraggingShape) {
    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
      glm::vec2 newShapePos = mousePos - m_dragOffset;
      m_mainSquare->SetPosition(newShapePos.x, newShapePos.y);
      if (m_luaEngine) {
        m_luaEngine->NotifyLuaShapePositionUpdated(newShapePos.x,
                                                   newShapePos.y);
      }
    } else { // Mouse button released
      m_isDraggingShape = false;
      // Optional: Restore original color if changed for dragging
      // m_mainSquare->SetColor(shapeColor.r, shapeColor.g, shapeColor.b,
      // shapeColor.a); // This needs the color *before* drag started
    }
  }
}

void Application::Render() {
  ImGui::Render();

  int display_w;
  int display_h;
  glfwGetFramebufferSize(m_window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);

  glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2],
               m_backgroundColor[3]);
  glClear(GL_COLOR_BUFFER_BIT);

  RenderScene();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(m_window);
}

void Application::RenderScene() {
  if (m_mainSquare) {
    m_mainSquare->Render(m_projectionMatrix);
  }
  // Later, if you have a list:
  // for (const auto& renderable : m_renderables) {
  //     if (renderable) renderable->Render(m_projectionMatrix);
  // }

  // Important: After rendering your scene objects that use a specific shader,
  // if ImGui uses a different shader (which it does), you might need to
  // ensure its state is restored. ImGui_ImplOpenGL3_RenderDrawData usually
  // handles this. If you see issues, you might explicitly call glUseProgram(0)
  // after your scene.
}

void Application::Shutdown() {
  CleanupRenderables();
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

void Application::CleanupRenderables() {
  if (m_mainSquare) {
    m_mainSquare->Cleanup();
    m_mainSquare.reset();
  }
  // for (auto& renderable : m_renderables) {
  //     if (renderable) renderable->Cleanup();
  // }
  // m_renderables.clear();

  m_simpleShapeShader.Cleanup(); // Cleanup the shader program
}

glm::vec2 Application::getWindowDimensions() {
  return glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
}

// --- Public setters for Lua ---
void Application::SetShapePosition(float x, float y) {
  if (m_mainSquare) {
    m_mainSquare->SetPosition(x, y);
  }
}

void Application::SetShapeSize(float size) {
  if (m_mainSquare) {
    m_mainSquare->SetSize(size);
  }
}

void Application::SetShapeColor(float r, float g, float b, float a) {
  if (m_mainSquare) {
    m_mainSquare->SetColor(r, g, b, a);
  }
}

void Application::SetBackgroundColor(float r, float g, float b, float a) {
  m_backgroundColor[0] = r;
  m_backgroundColor[1] = g;
  m_backgroundColor[2] = b;
  m_backgroundColor[3] = a;
}

glm::vec2 Application::GetShapePositionLua() const {
  if (m_mainSquare) {
    return m_mainSquare->GetPosition();
  }
  return {0.0f, 0.0f}; // Default if no square
}

float Application::GetShapeSizeLua() const {
  if (m_mainSquare) {
    return m_mainSquare->GetSize();
  }
  return 0.0f;
}

glm::vec4 Application::GetShapeColorLua() const {
  if (m_mainSquare) {
    return m_mainSquare->GetColor();
  }
  return {0.0f, 0.0f, 0.0f, 0.0f};
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
    else if (key == GLFW_KEY_R && ((mods & GLFW_MOD_CONTROL) != 0)) {
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