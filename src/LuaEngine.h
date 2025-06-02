#pragma once

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include <chrono>
#include <filesystem>

class Application;

class LuaEngine {
public:
  LuaEngine();
  ~LuaEngine();

  bool Initialize(Application *appInstance);
  void DrawGUI();

  // New live reload functionality
  void CheckForScriptReload();
  void SetAutoReload(bool enable) { m_autoReload = enable; }
  [[nodiscard]] bool IsAutoReloadEnabled() const { return m_autoReload; }
  void ForceReload();
  void NotifyLuaShapePositionUpdated(float x, float y);

private:
  void RegisterBindings();
  void CallLuaFunction(const char *functionName);
  void CreateDefaultGUI();

  static bool LoadScriptInternal(const char *filename, lua_State *targetL);
  bool ReloadScript();
  static std::filesystem::file_time_type
  GetFileModificationTime(const std::string &filename);

  lua_State *L;
  Application *m_app = nullptr;
  bool m_scriptLoaded;

  std::string m_scriptPath;
  std::filesystem::file_time_type m_lastModTime;
  bool m_autoReload;
  std::chrono::steady_clock::time_point m_lastCheckTime;
  static constexpr std::chrono::milliseconds CHECK_INTERVAL{
      500}; // Check every 500ms

  // Static Lua C functions for bindings
  static int Lua_SetShapePosition(lua_State *L);
  static int Lua_SetShapeSize(lua_State *L);
  static int Lua_SetShapeColor(lua_State *L);
  static int Lua_SetBackgroundColor(lua_State *L);
  static int Lua_GetShapePosition(lua_State *L);
  static int Lua_GetShapeSize(lua_State *L);
  static int Lua_GetShapeColor(lua_State *L);
};
