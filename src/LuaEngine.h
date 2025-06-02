#pragma once

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

class LuaEngine {
public:
  LuaEngine();
  ~LuaEngine();

  bool Initialize();
  void DrawGUI();
  bool LoadScript(const char *filename);

private:
  void RegisterBindings();
  void CallLuaFunction(const char *functionName);
  void CreateDefaultGUI();

  lua_State *L;
  bool m_scriptLoaded;
};