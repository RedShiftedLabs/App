#include "LuaEngine.h"
#include "ImGuiBindings.h"
#include <imgui.h>
#include <iostream>

LuaEngine::LuaEngine() : L(nullptr), m_scriptLoaded(false) {}

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
  m_scriptLoaded = LoadScript("gui.lua");
  if (!m_scriptLoaded) {
    std::cout << "No gui.lua found, using default GUI\n";
  }

  return true;
}

void LuaEngine::RegisterBindings() { ImGuiBindings::Register(L); }

void LuaEngine::DrawGUI() {
  if (m_scriptLoaded) {
    CallLuaFunction("draw_gui");
  } else {
    CreateDefaultGUI();
  }
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

  if (ImGui::Button("Click me!")) {
    counter++;
  }
  ImGui::Text("Clicked %d times", counter);

  ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);
  ImGui::End();
}