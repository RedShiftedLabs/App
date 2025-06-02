#pragma once

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

#include <chrono>
#include <filesystem>

class LuaEngine {
public:
  LuaEngine();
  ~LuaEngine();

  bool Initialize();
  void DrawGUI();
  bool LoadScript(const char *filename);

  // New live reload functionality
  void CheckForScriptReload();
  void SetAutoReload(bool enable) { m_autoReload = enable; }
  [[nodiscard]] bool IsAutoReloadEnabled() const { return m_autoReload; }
  void ForceReload();

private:
  void RegisterBindings();
  void CallLuaFunction(const char *functionName);
  void CreateDefaultGUI();

  // New helper methods
  bool ReloadScript();
  static std::filesystem::file_time_type
  GetFileModificationTime(const std::string &filename);

  lua_State *L;
  bool m_scriptLoaded;

  // Live reload members
  std::string m_scriptPath;
  std::filesystem::file_time_type m_lastModTime;
  bool m_autoReload;
  std::chrono::steady_clock::time_point m_lastCheckTime;
  static constexpr std::chrono::milliseconds CHECK_INTERVAL{
      500}; // Check every 500ms
};
