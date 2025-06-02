#pragma once

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

class ImGuiBindings {
public:
  static void Register(lua_State *L);

private:
  // Window functions
  static int Begin(lua_State *L);
  static int End(lua_State *L);

  // Text functions
  static int Text(lua_State *L);
  static int TextColored(lua_State *L);
  static int BulletText(lua_State *L);

  // Button/Input functions
  static int Button(lua_State *L);
  static int SmallButton(lua_State *L);
  static int Checkbox(lua_State *L);
  static int RadioButton(lua_State *L);

  // Slider/Input functions
  static int SliderFloat(lua_State *L);
  static int SliderInt(lua_State *L);
  static int InputText(lua_State *L);
  static int InputFloat(lua_State *L);
  static int InputInt(lua_State *L);

  // Layout functions
  static int Separator(lua_State *L);
  static int SameLine(lua_State *L);
  static int Spacing(lua_State *L);
  static int Indent(lua_State *L);
  static int Unindent(lua_State *L);

  // Tree/Collapsing functions
  static int TreeNode(lua_State *L);
  static int TreePop(lua_State *L);
  static int CollapsingHeader(lua_State *L);

  // Color functions
  static int ColorEdit3(lua_State *L);
  static int ColorEdit4(lua_State *L);

  // Combo/List functions
  static int BeginCombo(lua_State *L);
  static int EndCombo(lua_State *L);
  static int Selectable(lua_State *L);

  // Menu functions
  static int BeginMenuBar(lua_State *L);
  static int EndMenuBar(lua_State *L);
  static int BeginMenu(lua_State *L);
  static int EndMenu(lua_State *L);
  static int MenuItem(lua_State *L);

  // Utility functions
  static int GetWindowSize(lua_State *L);
  static int SetWindowSize(lua_State *L);
  static int GetWindowPos(lua_State *L);
  static int SetWindowPos(lua_State *L);

  // Helper functions for Lua table handling
  static void PushColor(lua_State *L, float r, float g, float b,
                        float a = 1.0f);
  static bool GetColorFromTable(lua_State *L, int index, float &r, float &g,
                                float &b, float &a);
};