cmake_minimum_required(VERSION 3.15)
project(App)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

add_subdirectory(deps/glfw)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

# Try to find Lua using pkg-config first (more reliable on macOS)
pkg_check_modules(LUA_PKG QUIET lua5.4)
if(NOT LUA_PKG_FOUND)
    pkg_check_modules(LUA_PKG QUIET lua-5.4)
endif()
if(NOT LUA_PKG_FOUND)
    pkg_check_modules(LUA_PKG QUIET lua5.3)
endif()
if(NOT LUA_PKG_FOUND)
    pkg_check_modules(LUA_PKG QUIET lua-5.3)
endif()
if(NOT LUA_PKG_FOUND)
    pkg_check_modules(LUA_PKG QUIET lua)
endif()

if(LUA_PKG_FOUND)
    set(LUA_INCLUDE_DIRS ${LUA_PKG_INCLUDE_DIRS})
    set(LUA_LIBRARIES ${LUA_PKG_LIBRARIES})
    set(LUA_LIBRARY_DIRS ${LUA_PKG_LIBRARY_DIRS})
    set(LUA_CFLAGS_OTHER ${LUA_PKG_CFLAGS_OTHER})
    message(STATUS "Found Lua via PkgConfig: Headers=${LUA_INCLUDE_DIRS} Libs=${LUA_LIBRARIES}")
else()
    # Fallback to find_package
    find_package(Lua 5.4 QUIET)
    if(NOT Lua_FOUND)
        find_package(Lua 5.3 QUIET)
    endif()
    if(NOT Lua_FOUND)
        find_package(Lua REQUIRED)
    endif()

    if(Lua_FOUND)
        message(STATUS "Found Lua via find_package: Version=${LUA_VERSION_STRING} Headers=${LUA_INCLUDE_DIR} Libs=${LUA_LIBRARIES}")
        set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "Lua not found by find_package or PkgConfig.")
    endif()
endif()

set(IMGUI_SOURCES
    deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
)

set(APP_SOURCES
    src/main.cpp
    src/Application.cpp
    src/LuaEngine.cpp
    src/ImGuiBindings.cpp
)

add_executable(App
    ${APP_SOURCES}
    ${IMGUI_SOURCES}
)

# Create symbolic link instead of copying (better for hot reload)
if(WIN32)
    # Windows: Use copy for compatibility
    add_custom_command(TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/gui.lua
        $<TARGET_FILE_DIR:App>/gui.lua
    )
else()
    # Unix/Linux/macOS: Use symbolic link
    add_custom_command(TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_SOURCE_DIR}/gui.lua
        $<TARGET_FILE_DIR:App>/gui.lua
    )
endif()

target_include_directories(App PRIVATE
    src
    deps/glfw/include
    deps/glm
    deps/imgui
    deps/imgui/backends
    ${LUA_INCLUDE_DIRS}
)

# Link libraries in the correct order
target_link_libraries(App PRIVATE
    glfw
    OpenGL::GL
)

# Add Lua library directories before linking
if(LUA_PKG_FOUND AND LUA_LIBRARY_DIRS)
    target_link_directories(App PRIVATE ${LUA_LIBRARY_DIRS})
endif()

# Link Lua library
target_link_libraries(App PRIVATE ${LUA_LIBRARIES})

# Apply Lua compile flags
if(LUA_PKG_FOUND AND LUA_CFLAGS_OTHER)
    target_compile_options(App PRIVATE ${LUA_CFLAGS_OTHER})
endif()

# macOS specific frameworks
if(APPLE)
    target_link_libraries(App PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation"
    )
endif()