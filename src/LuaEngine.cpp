// LuaEngine.cpp
#include "LuaEngine.h"
#include "ImGuiBindings.h"
#include <filesystem>
#include <imgui.h>
#include <iostream>

LuaEngine::LuaEngine()
    : L(nullptr), m_scriptLoaded(false), m_autoReload(true),
      m_lastCheckTime(std::chrono::steady_clock::now()) {}

LuaEngine::~LuaEngine() {
  if (L != nullptr) {
    lua_close(L);
  }
}

bool LuaEngine::Initialize() {
  L = luaL_newstate();
  if (L == nullptr) {
    std::cerr << "Failed to create Lua state\n";
    return false;
  }

  luaL_openlibs(L);
  RegisterBindings();

  // Try to load GUI script
  m_scriptPath = "gui.lua";
  m_scriptLoaded = LoadScript(m_scriptPath.c_str());
  if (!m_scriptLoaded) {
    std::cout << "No gui.lua found, using default GUI\n";
  } else {
    // Store initial modification time
    try {
      m_lastModTime = GetFileModificationTime(m_scriptPath);
    } catch (const std::exception &e) {
      std::cerr << "Warning: Could not get file modification time: " << e.what()
                << "\n";
    }
  }

  return true;
}

void LuaEngine::RegisterBindings() { ImGuiBindings::Register(L); }

void LuaEngine::DrawGUI() {
  // Check for script reload if auto-reload is enabled
  if (m_autoReload && m_scriptLoaded) {
    CheckForScriptReload();
  }

  if (m_scriptLoaded) {
    CallLuaFunction("draw_gui");
  } else {
    CreateDefaultGUI();
  }
}

void LuaEngine::CheckForScriptReload() {
  auto now = std::chrono::steady_clock::now();

  // Only check periodically to avoid excessive file system calls
  if (now - m_lastCheckTime < CHECK_INTERVAL) {
    return;
  }

  m_lastCheckTime = now;

  try {
    auto currentModTime = GetFileModificationTime(m_scriptPath);

    if (currentModTime != m_lastModTime) {
      std::cout << "Script file changed, reloading...\n";
      if (ReloadScript()) {
        m_lastModTime = currentModTime;
        std::cout << "Script reloaded successfully!\n";
      } else {
        std::cerr << "Failed to reload script, keeping previous version\n";
      }
    }
  } catch (const std::exception &e) {
    // File might have been deleted or moved
    std::cerr << "Error checking script file: " << e.what() << "\n";
  }
}

void LuaEngine::ForceReload() {
  if (m_scriptLoaded) {
    std::cout << "Force reloading script...\n";
    if (ReloadScript()) {
      try {
        m_lastModTime = GetFileModificationTime(m_scriptPath);
      } catch (const std::exception &e) {
        std::cerr << "Warning: Could not update modification time: " << e.what()
                  << "\n";
      }
      std::cout << "Script force reloaded successfully!\n";
    } else {
      std::cerr << "Failed to force reload script\n";
    }
  }
}

bool LuaEngine::ReloadScript() {
  // Create a new Lua state for reloading
  lua_State *newL = luaL_newstate();
  if (newL == nullptr) {
    std::cerr << "Failed to create new Lua state for reload\n";
    return false;
  }

  luaL_openlibs(newL);

  // Re-register bindings
  ImGuiBindings::Register(newL);

  // Try to load the script
  int result = luaL_loadfile(newL, m_scriptPath.c_str());
  if (result != LUA_OK) {
    std::cerr << "Failed to load script for reload: " << lua_tostring(newL, -1)
              << "\n";
    lua_close(newL);
    return false;
  }

  result = lua_pcall(newL, 0, 0, 0);
  if (result != LUA_OK) {
    std::cerr << "Error executing reloaded script: " << lua_tostring(newL, -1)
              << "\n";
    lua_close(newL);
    return false;
  }

  // If we got here, the reload was successful
  // Close the old state and replace it
  lua_close(L);
  L = newL;

  return true;
}

std::filesystem::file_time_type
LuaEngine::GetFileModificationTime(const std::string &filename) {
  std::error_code ec;
  auto modTime = std::filesystem::last_write_time(filename, ec);
  if (ec) {
    throw std::runtime_error("Could not get file modification time: " +
                             ec.message());
  }
  return modTime;
}

bool LuaEngine::LoadScript(const char *filename) {
  int result = luaL_loadfile(L, filename);
  if (result != LUA_OK) {
    return false;
  }

  result = lua_pcall(L, 0, 0, 0);
  if (result != LUA_OK) {
    std::cerr << "Error executing script: " << lua_tostring(L, -1) << "\n";
    lua_pop(L, 1);
    return false;
  }

  return true;
}

void LuaEngine::CallLuaFunction(const char *functionName) {
  lua_getglobal(L, functionName);
  if (lua_isfunction(L, -1)) {
    int result = lua_pcall(L, 0, 0, 0);
    if (result != LUA_OK) {
      std::cerr << "Error calling " << functionName << ": "
                << lua_tostring(L, -1) << "\n";
      lua_pop(L, 1);
    }
  } else {
    lua_pop(L, 1);
  }
}

void LuaEngine::CreateDefaultGUI() {
  static int counter = 0;
  static float value = 0.5f;

  ImGui::Begin("Default GUI");
  ImGui::Text("This is the default C++ GUI");
  ImGui::Text("Create gui.lua to override this");
  ImGui::Separator();

  // Live reload controls
  ImGui::Text("Live Reload Controls:");
  bool autoReload = m_autoReload;
  if (ImGui::Checkbox("Auto-reload enabled", &autoReload)) {
    SetAutoReload(autoReload);
  }

  ImGui::SameLine();
  if (ImGui::Button("Force Reload")) {
    ForceReload();
  }

  ImGui::Separator();

  if (ImGui::Button("Click me!")) {
    counter++;
  }
  ImGui::Text("Clicked %d times", counter);

  ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);
  ImGui::End();
}