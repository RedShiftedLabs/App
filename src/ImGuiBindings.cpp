// ImGuiBindings.cpp
#include "ImGuiBindings.h"
#include <cstring>
#include <imgui.h>
#include <lauxlib.h>

static float g_bgColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};

using LuaFunction = luaL_Reg;

void ImGuiBindings::Register(lua_State *l) {
  static const LuaFunction functions[] = {
      // Window functions
      {"Begin", Begin},
      {"End", End},

      // Text functions
      {"Text", Text},
      {"TextColored", TextColored},
      {"BulletText", BulletText},

      // Button/Input functions
      {"Button", Button},
      {"SmallButton", SmallButton},
      {"Checkbox", Checkbox},
      {"RadioButton", RadioButton},

      // Slider/Input functions
      {"SliderFloat", SliderFloat},
      {"SliderInt", SliderInt},
      {"InputText", InputText},
      {"InputFloat", InputFloat},
      {"InputInt", InputInt},

      // Layout functions
      {"Separator", Separator},
      {"SameLine", SameLine},
      {"Spacing", Spacing},
      {"Indent", Indent},
      {"Unindent", Unindent},

      // Tree/Collapsing functions
      {"TreeNode", TreeNode},
      {"TreePop", TreePop},
      {"CollapsingHeader", CollapsingHeader},

      // Color functions
      {"ColorEdit3", ColorEdit3},
      {"ColorEdit4", ColorEdit4},

      // Combo/List functions
      {"BeginCombo", BeginCombo},
      {"EndCombo", EndCombo},
      {"Selectable", Selectable},

      // Menu functions
      {"BeginMenuBar", BeginMenuBar},
      {"EndMenuBar", EndMenuBar},
      {"BeginMenu", BeginMenu},
      {"EndMenu", EndMenu},
      {"MenuItem", MenuItem},

      // Utility functions
      {"GetWindowSize", GetWindowSize},
      {"SetWindowSize", SetWindowSize},
      {"GetWindowPos", GetWindowPos},
      {"SetWindowPos", SetWindowPos},

      {nullptr, nullptr}};

  // NOLINTBEGIN(readability-math-missing-parentheses)
  luaL_newlib(l, functions);
  // NOLINTEND(readability-math-missing-parentheses)
  lua_setglobal(l, "ImGui");
}

// Window functions
int ImGuiBindings::Begin(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  bool *p_open = nullptr;
  ImGuiWindowFlags flags = 0;

  // Optional second parameter for p_open
  if (lua_gettop(L) > 1 && !lua_isnil(L, 2)) {
    // Handle p_open if needed
  }

  // Optional third parameter for flags
  if (lua_gettop(L) > 2) {
    flags = luaL_checkinteger(L, 3);
  }

  bool result = ImGui::Begin(name, p_open, flags);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

int ImGuiBindings::End(lua_State *L) {
  ImGui::End();
  return 0;
}

// Text functions
int ImGuiBindings::Text(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  ImGui::Text("%s", text);
  return 0;
}

int ImGuiBindings::TextColored(lua_State *L) {
  float r;
  float g;
  float b;
  float a = 1.0f;
  if (!GetColorFromTable(L, 1, r, g, b, a)) {
    return luaL_error(L, "Expected color table {r, g, b, a}");
  }
  const char *text = luaL_checkstring(L, 2);
  ImGui::TextColored(ImVec4(r, g, b, a), "%s", text);
  return 0;
}

int ImGuiBindings::BulletText(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  ImGui::BulletText("%s", text);
  return 0;
}

// Button functions
int ImGuiBindings::Button(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  ImVec2 size(0, 0);

  if (lua_gettop(L) > 1) {
    size.x = luaL_checknumber(L, 2);
    size.y = luaL_checknumber(L, 3);
  }

  bool pressed = ImGui::Button(text, size);
  lua_pushboolean(L, static_cast<int>(pressed));
  return 1;
}

int ImGuiBindings::SmallButton(lua_State *L) {
  const char *text = luaL_checkstring(L, 1);
  bool pressed = ImGui::SmallButton(text);
  lua_pushboolean(L, static_cast<int>(pressed));
  return 1;
}

int ImGuiBindings::Checkbox(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  bool value = lua_toboolean(L, 2) != 0;

  bool changed = ImGui::Checkbox(label, &value);
  lua_pushboolean(L, static_cast<int>(changed));
  lua_pushboolean(L, static_cast<int>(value));
  return 2;
}

int ImGuiBindings::RadioButton(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  bool active = lua_toboolean(L, 2) != 0;

  bool pressed = ImGui::RadioButton(label, active);
  lua_pushboolean(L, static_cast<int>(pressed));
  return 1;
}

// Slider functions
int ImGuiBindings::SliderFloat(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  float value = luaL_checknumber(L, 2);
  float min_val = luaL_checknumber(L, 3);
  float max_val = luaL_checknumber(L, 4);

  bool changed = ImGui::SliderFloat(label, &value, min_val, max_val);
  lua_pushboolean(L, static_cast<int>(changed));
  lua_pushnumber(L, value);
  return 2;
}

int ImGuiBindings::SliderInt(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  int value = luaL_checkinteger(L, 2);
  int min_val = luaL_checkinteger(L, 3);
  int max_val = luaL_checkinteger(L, 4);

  bool changed = ImGui::SliderInt(label, &value, min_val, max_val);
  lua_pushboolean(L, static_cast<int>(changed));
  lua_pushinteger(L, value);
  return 2;
}

// Input functions
int ImGuiBindings::InputText(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  const char *text = luaL_checkstring(L, 2);

  // Create a buffer for text input
  static char buffer[256];
  strncpy(buffer, text, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  bool changed = ImGui::InputText(label, buffer, sizeof(buffer));
  lua_pushboolean(L, static_cast<int>(changed));
  lua_pushstring(L, buffer);
  return 2;
}

int ImGuiBindings::InputFloat(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  float value = luaL_checknumber(L, 2);

  bool changed = ImGui::InputFloat(label, &value);
  lua_pushboolean(L, static_cast<int>(changed));
  lua_pushnumber(L, value);
  return 2;
}

int ImGuiBindings::InputInt(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  int value = luaL_checkinteger(L, 2);

  bool changed = ImGui::InputInt(label, &value);
  lua_pushboolean(L, static_cast<int>(changed));
  lua_pushinteger(L, value);
  return 2;
}

// Layout functions
int ImGuiBindings::Separator(lua_State *L) {
  ImGui::Separator();
  return 0;
}

int ImGuiBindings::SameLine(lua_State *L) {
  float offset_from_start_x = 0.0f;
  float spacing = -1.0f;

  if (lua_gettop(L) > 0) {
    offset_from_start_x = luaL_checknumber(L, 1);
  }
  if (lua_gettop(L) > 1) {
    spacing = luaL_checknumber(L, 2);
  }

  ImGui::SameLine(offset_from_start_x, spacing);
  return 0;
}

int ImGuiBindings::Spacing(lua_State *L) {
  ImGui::Spacing();
  return 0;
}

int ImGuiBindings::Indent(lua_State *L) {
  float indent_w = 0.0f;
  if (lua_gettop(L) > 0) {
    indent_w = luaL_checknumber(L, 1);
  }
  ImGui::Indent(indent_w);
  return 0;
}

int ImGuiBindings::Unindent(lua_State *L) {
  float indent_w = 0.0f;
  if (lua_gettop(L) > 0) {
    indent_w = luaL_checknumber(L, 1);
  }
  ImGui::Unindent(indent_w);
  return 0;
}

// Tree functions
int ImGuiBindings::TreeNode(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  bool result = ImGui::TreeNode(label);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

int ImGuiBindings::TreePop(lua_State *L) {
  ImGui::TreePop();
  return 0;
}

int ImGuiBindings::CollapsingHeader(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  bool result = ImGui::CollapsingHeader(label);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

// Color functions
int ImGuiBindings::ColorEdit3(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  float col[3];

  // Get color from table
  if (!lua_istable(L, 2)) {
    return luaL_error(L, "Expected color table");
  }

  for (int i = 0; i < 3; i++) {
    lua_pushinteger(L, i + 1);
    lua_gettable(L, 2);
    col[i] = luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }

  bool changed = ImGui::ColorEdit3(label, col);

  // Return results
  lua_pushboolean(L, static_cast<int>(changed));
  lua_newtable(L);
  for (int i = 0; i < 3; i++) {
    lua_pushinteger(L, i + 1);
    lua_pushnumber(L, col[i]);
    lua_settable(L, -3);
  }

  return 2;
}

int ImGuiBindings::ColorEdit4(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  float col[4];

  // Get color from table
  if (!lua_istable(L, 2)) {
    return luaL_error(L, "Expected color table");
  }

  for (int i = 0; i < 4; i++) {
    lua_pushinteger(L, i + 1);
    lua_gettable(L, 2);
    col[i] = luaL_checknumber(L, -1);
    lua_pop(L, 1);
  }

  bool changed = ImGui::ColorEdit4(label, col);

  // Return results
  lua_pushboolean(L, static_cast<int>(changed));
  lua_newtable(L);
  for (int i = 0; i < 4; i++) {
    lua_pushinteger(L, i + 1);
    lua_pushnumber(L, col[i]);
    lua_settable(L, -3);
  }

  return 2;
}

// Combo functions
int ImGuiBindings::BeginCombo(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  const char *preview_value = luaL_checkstring(L, 2);

  bool result = ImGui::BeginCombo(label, preview_value);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

int ImGuiBindings::EndCombo(lua_State *L) {
  ImGui::EndCombo();
  return 0;
}

int ImGuiBindings::Selectable(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  bool selected = false;

  if (lua_gettop(L) > 1) {
    selected = (lua_toboolean(L, 2) != 0);
  }

  bool result = ImGui::Selectable(label, selected);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

// Menu functions
int ImGuiBindings::BeginMenuBar(lua_State *L) {
  bool result = ImGui::BeginMenuBar();
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

int ImGuiBindings::EndMenuBar(lua_State *L) {
  ImGui::EndMenuBar();
  return 0;
}

int ImGuiBindings::BeginMenu(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  bool enabled = true;

  if (lua_gettop(L) > 1) {
    enabled = (lua_toboolean(L, 2) != 0);
  }

  bool result = ImGui::BeginMenu(label, enabled);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

int ImGuiBindings::EndMenu(lua_State *L) {
  ImGui::EndMenu();
  return 0;
}

int ImGuiBindings::MenuItem(lua_State *L) {
  const char *label = luaL_checkstring(L, 1);
  const char *shortcut = nullptr;
  bool selected = false;
  bool enabled = true;

  if (lua_gettop(L) > 1 && !lua_isnil(L, 2)) {
    shortcut = lua_tostring(L, 2);
  }
  if (lua_gettop(L) > 2) {
    selected = (lua_toboolean(L, 3) != 0);
  }
  if (lua_gettop(L) > 3) {
    enabled = (lua_toboolean(L, 4) != 0);
  }

  bool result = ImGui::MenuItem(label, shortcut, selected, enabled);
  lua_pushboolean(L, static_cast<int>(result));
  return 1;
}

// Utility functions
int ImGuiBindings::GetWindowSize(lua_State *L) {
  ImVec2 size = ImGui::GetWindowSize();
  lua_pushnumber(L, size.x);
  lua_pushnumber(L, size.y);
  return 2;
}

int ImGuiBindings::SetWindowSize(lua_State *L) {
  float width = luaL_checknumber(L, 1);
  float height = luaL_checknumber(L, 2);
  ImGui::SetWindowSize(ImVec2(width, height));
  return 0;
}

int ImGuiBindings::GetWindowPos(lua_State *L) {
  ImVec2 pos = ImGui::GetWindowPos();
  lua_pushnumber(L, pos.x);
  lua_pushnumber(L, pos.y);
  return 2;
}

int ImGuiBindings::SetWindowPos(lua_State *L) {
  float x = luaL_checknumber(L, 1);
  float y = luaL_checknumber(L, 2);
  ImGui::SetWindowPos(ImVec2(x, y));
  return 0;
}

// Helper functions
void ImGuiBindings::PushColor(lua_State *L, float r, float g, float b,
                              float a) {
  lua_newtable(L);
  lua_pushnumber(L, r);
  lua_rawseti(L, -2, 1);
  lua_pushnumber(L, g);
  lua_rawseti(L, -2, 2);
  lua_pushnumber(L, b);
  lua_rawseti(L, -2, 3);
  lua_pushnumber(L, a);
  lua_rawseti(L, -2, 4);
}

bool ImGuiBindings::GetColorFromTable(lua_State *L, int index, float &r,
                                      float &g, float &b, float &a) {
  if (!lua_istable(L, index)) {
    return false;
  }

  lua_pushinteger(L, 1);
  lua_gettable(L, index);
  r = luaL_checknumber(L, -1);
  lua_pop(L, 1);
  lua_pushinteger(L, 2);
  lua_gettable(L, index);
  g = luaL_checknumber(L, -1);
  lua_pop(L, 1);
  lua_pushinteger(L, 3);
  lua_gettable(L, index);
  b = luaL_checknumber(L, -1);
  lua_pop(L, 1);

  // Alpha is optional
  lua_pushinteger(L, 4);
  lua_gettable(L, index);
  if (!lua_isnil(L, -1)) {
    a = luaL_checknumber(L, -1);
  }
  lua_pop(L, 1);

  return true;
}