// LuaEngine.cpp
#include "LuaEngine.h"
#include "Application.h"
#include "ImGuiBindings.h"
#include <filesystem>
#include <imgui.h>
#include <iostream>

static Application *GetAppInstanceFromLua(lua_State *L) {
  // Key used in RegisterBindings
  lua_getfield(L, LUA_REGISTRYINDEX, "AppInstance");
  Application *app = static_cast<Application *>(lua_touserdata(L, -1));
  lua_pop(L, 1); // Pop the userdata
  return app;
}

// Lua C Functions Implementation
int LuaEngine::Lua_SetShapePosition(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) {
    return luaL_error(L, "App instance not found");
  }
  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  app->SetShapePosition(x, y);
  return 0;
}

int LuaEngine::Lua_SetShapeSize(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) {
    return luaL_error(L, "App instance not found");
  }
  float size = luaL_checknumber(L, 1);
  app->SetShapeSize(size);
  return 0;
}

int LuaEngine::Lua_SetShapeColor(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) {
    return luaL_error(L, "App instance not found");
  }

  float r;
  float g;
  float b;
  float a = 1.0f; // Default alpha to 1.0
  int n = lua_gettop(L);

  if (lua_istable(L, 1)) {
    lua_rawgeti(L, 1, 1);
    r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 1, 2);
    g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 1, 3);
    b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    // Check for alpha in table
    lua_rawgeti(L, 1, 4);
    if (!lua_isnil(L, -1)) {
      a = luaL_checknumber(L, -1);
    }
    lua_pop(L, 1);
  } else {
    r = luaL_checknumber(L, 1);
    g = luaL_checknumber(L, 2);
    b = luaL_checknumber(L, 3);
    if (n >= 4) { // Check if 4th argument (alpha) is provided
      a = luaL_checknumber(L, 4);
    }
  }
  app->SetShapeColor(r, g, b, a);
  return 0;
}

int LuaEngine::Lua_SetBackgroundColor(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) {
    return luaL_error(
        L, "Application instance not found in Lua_SetBackgroundColor");
  }

  float r;
  float g;
  float b;
  float a;
  if (lua_istable(L, 1)) { // Prefer table {r,g,b,a}
    lua_rawgeti(L, 1, 1);
    r = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 1, 2);
    g = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 1, 3);
    b = luaL_checknumber(L, -1);
    lua_pop(L, 1);
    lua_rawgeti(L, 1, 4);
    a = luaL_checknumber(L, -1);
    lua_pop(L, 1);
  } else { // Fallback to 4 separate arguments
    r = luaL_checknumber(L, 1);
    g = luaL_checknumber(L, 2);
    b = luaL_checknumber(L, 3);
    a = luaL_checknumber(L, 4);
  }
  app->SetBackgroundColor(r, g, b, a);
  return 0;
}

LuaEngine::LuaEngine()
    : L(nullptr), m_scriptLoaded(false), m_autoReload(true),
      m_lastCheckTime(std::chrono::steady_clock::now()) {}

LuaEngine::~LuaEngine() {
  if (L != nullptr) {
    lua_close(L);
  }
}

bool LuaEngine::Initialize(Application *appInstance) {
  m_app = appInstance; // Store the Application instance
  if (m_app == nullptr) {
    std::cerr << "LuaEngine::Initialize: Application instance is null\n";
    return false;
  }

  L = luaL_newstate();
  if (L == nullptr) {
    std::cerr << "Failed to create Lua state\n";
    return false;
  }

  luaL_openlibs(L);
  RegisterBindings();

  // Try to load GUI script
  m_scriptPath = "gui.lua";
  m_scriptLoaded = LoadScriptInternal(m_scriptPath.c_str(), L);
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

int LuaEngine::Lua_GetShapePosition(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) {
    lua_pushnil(L);
    lua_pushnil(L);
    return 2;
  } // Or error
  glm::vec2 pos = app->GetShapePositionLua();
  lua_pushnumber(L, pos.x);
  lua_pushnumber(L, pos.y);
  return 2; // Returns x, y
}

int LuaEngine::Lua_GetShapeSize(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) {
    lua_pushnil(L);
    return 1;
  }
  float size = app->GetShapeSizeLua();
  lua_pushnumber(L, size);
  return 1;
}

int LuaEngine::Lua_GetShapeColor(lua_State *L) {
  Application *app = GetAppInstanceFromLua(L);
  if (app == nullptr) { /* push nil table or error */
    return 1;
  }
  glm::vec4 color = app->GetShapeColorLua();
  lua_newtable(L); // Create table to return {r,g,b,a}
  lua_pushnumber(L, color.r);
  lua_rawseti(L, -2, 1);
  lua_pushnumber(L, color.g);
  lua_rawseti(L, -2, 2);
  lua_pushnumber(L, color.b);
  lua_rawseti(L, -2, 3);
  lua_pushnumber(L, color.a);
  lua_rawseti(L, -2, 4);
  return 1; // Returns the table
}

void LuaEngine::RegisterBindings() {
  ImGuiBindings::Register(L);

  lua_pushlightuserdata(L, m_app);
  lua_setfield(L, LUA_REGISTRYINDEX, "AppInstance");

  lua_newtable(L); // Creates the 'App' table
  static const luaL_Reg app_functions[] = {
      {"SetShapePosition", Lua_SetShapePosition},
      {"SetShapeSize", Lua_SetShapeSize},
      {"SetShapeColor", Lua_SetShapeColor},
      {"GetShapePosition", Lua_GetShapePosition},
      {"GetShapeSize", Lua_GetShapeSize},
      {"GetShapeColor", Lua_GetShapeColor},
      {"SetBackgroundColor", Lua_SetBackgroundColor},
      {nullptr, nullptr}};
  luaL_setfuncs(L, app_functions, 0);
  lua_setglobal(L, "App"); // Sets the table as a global named "App"
}

bool LuaEngine::LoadScriptInternal(const char *filename, lua_State *targetL) {
  if (targetL == nullptr) {
    return false;
  }
  int result = luaL_loadfile(targetL, filename);
  if (result != LUA_OK) {
    std::cerr << "Failed to load script '" << filename
              << "': " << lua_tostring(targetL, -1) << "\n";
    lua_pop(targetL, 1); // Pop error message
    return false;
  }

  result = lua_pcall(targetL, 0, 0, 0);
  if (result != LUA_OK) {
    std::cerr << "Error executing script '" << filename
              << "': " << lua_tostring(targetL, -1) << "\n";
    lua_pop(targetL, 1); // Pop error message
    return false;
  }
  return true;
}

void LuaEngine::DrawGUI() {
  // Check for script reload if auto-reload is enabled
  if (m_autoReload && m_scriptPath.empty()) {
    CheckForScriptReload();
  }

  if (m_scriptLoaded) {
    CallLuaFunction("draw_gui");
  } else {
    CreateDefaultGUI();
  }
}

void LuaEngine::CheckForScriptReload() {
  if (m_scriptPath.empty()) {
    return;
  }

  auto now = std::chrono::steady_clock::now();
  if (now - m_lastCheckTime < CHECK_INTERVAL) {
    return;
  }
  m_lastCheckTime = now;

  try {
    if (!std::filesystem::exists(m_scriptPath)) {
      if (m_scriptLoaded) { // Only print if it was previously loaded
        std::cerr << "Script file " << m_scriptPath
                  << " no longer exists. Keeping current version.\n";
      }
      // m_scriptLoaded = false; // Or some other handling
      return;
    }
    auto currentModTime = GetFileModificationTime(m_scriptPath);

    if (currentModTime != m_lastModTime) {
      std::cout << "Script file " << m_scriptPath << " changed, reloading...\n";
      if (ReloadScript()) {
        m_lastModTime = currentModTime;
        std::cout << "Script reloaded successfully!\n";
      } else {
        std::cerr << "Failed to reload script, keeping previous version\n";
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "Error checking script file " << m_scriptPath << ": "
              << e.what() << "\n";
  }
}

void LuaEngine::ForceReload() {
  if (m_scriptPath.empty()) {
    std::cout << "No script path set, cannot force reload.\n";
    return;
  }
  if (!std::filesystem::exists(m_scriptPath)) {
    std::cerr << "Script file " << m_scriptPath
              << " does not exist. Cannot force reload.\n";
    m_scriptLoaded = false; // Mark as not loaded if file is gone
    return;
  }

  std::cout << "Force reloading script " << m_scriptPath << "...\n";
  if (ReloadScript()) {
    try {
      m_lastModTime = GetFileModificationTime(m_scriptPath);
      std::cout << "Script force reloaded successfully!\n";
    } catch (const std::exception &e) {
      std::cerr << "Warning: Could not update modification time for "
                << m_scriptPath << ": " << e.what() << "\n";
    }
  } else {
    std::cerr << "Failed to force reload script " << m_scriptPath << "\n";
  }
}

bool LuaEngine::ReloadScript() {

  lua_State *newL = luaL_newstate();
  if (newL == nullptr) {
    std::cerr << "Failed to create new Lua state for reload\n";
    return false;
  }

  luaL_openlibs(newL);

  // Store Application pointer in the new Lua state's registry
  lua_pushlightuserdata(newL, m_app);
  lua_setfield(newL, LUA_REGISTRYINDEX, "AppInstance");

  ImGuiBindings::Register(newL);
  lua_newtable(newL); // Creates the 'App' table in the NEW Lua state
  static const luaL_Reg app_functions[] = {
      {"SetShapePosition", Lua_SetShapePosition},
      {"SetShapeSize", Lua_SetShapeSize},
      {"SetShapeColor", Lua_SetShapeColor},
      {"GetShapePosition", Lua_GetShapePosition},
      {"GetShapeSize", Lua_GetShapeSize},
      {"GetShapeColor", Lua_GetShapeColor},
      {"SetBackgroundColor", Lua_SetBackgroundColor},
      {nullptr, nullptr}};
  luaL_setfuncs(newL, app_functions, 0);
  lua_setglobal(newL, "App");
  // --- End of re-registering App bindings

  if (!LoadScriptInternal(m_scriptPath.c_str(), newL)) {
    lua_close(newL);
    return false; // LoadScriptInternal already prints errors
  }

  // If we got here, the reload was successful
  if (L != nullptr) {
    lua_close(L); // Close the old state
  }
  L = newL;              // Replace with the new state
  m_scriptLoaded = true; // Mark as loaded

  return true;
}

void LuaEngine::NotifyLuaShapePositionUpdated(float x, float y) {
  if ((L == nullptr) || !m_scriptLoaded) {
    return;
  }

  lua_getglobal(L, "OnCppShapePositionUpdated");
  if (lua_isfunction(L, -1)) {
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
      lua_pop(L, 1);
    }
  } else {
    lua_pop(L, 1);
  }
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
  ImGui::Text("Create gui.lua to override this, or if gui.lua failed to load.");
  ImGui::Separator();

  // Live reload controls
  ImGui::Text("Live Reload Controls:");
  bool autoReload = m_autoReload;
  if (ImGui::Checkbox("Auto-reload enabled", &autoReload)) {
    SetAutoReload(autoReload);
  }

  ImGui::SameLine();
  if (ImGui::Button("Force Reload Script")) { // Renamed for clarity
    ForceReload();
  }

  ImGui::Separator();

  if (ImGui::Button("Click me!")) {
    counter++;
  }
  ImGui::Text("Clicked %d times", counter);

  ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);

  // Example: If you want to control the square from default C++ GUI
  // This would require m_app to be valid.
  if (m_app != nullptr) {
    ImGui::Separator();
    ImGui::Text("Square Controls (C++ Default):");
    static float sq_x = 150.0f;
    static float sq_y = 150.0f;
    static float sq_size = 50.0f;
    static float sq_col[3] = {1.0f, 0.0f, 0.0f};

    bool changed = false;
    changed |= ImGui::SliderFloat("Default Sq X", &sq_x, 0,
                                  Application::getWindowDimensions().x);
    changed |= ImGui::SliderFloat("Default Sq Y", &sq_y, 0,
                                  Application::getWindowDimensions().y);
    if (changed) {
      m_app->SetShapePosition(sq_x, sq_y);
    }

    if (ImGui::SliderFloat("Default Sq Size", &sq_size, 10, 200)) {
      m_app->SetShapeSize(sq_size);
    }
    if (ImGui::ColorEdit3("Default Sq Color", sq_col)) {
      m_app->SetShapeColor(sq_col[0], sq_col[1], sq_col[2]);
    }
  }

  ImGui::End();
}