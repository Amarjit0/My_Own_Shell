cmake_minimum_required(VERSION 3.20)
project(gupt VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection
if(WIN32)
    add_definitions(-DKS_PLATFORM_WINDOWS)
    set(PLATFORM_SOURCES src/utils/platform_win.c)
    set(PLATFORM_LIBS ws2_32)
elseif(UNIX)
    add_definitions(-DKS_PLATFORM_UNIX)
    set(PLATFORM_SOURCES src/utils/platform.c)
    set(PLATFORM_LIBS dl m)
endif()

# Find required packages
find_package(PkgConfig QUIET)

# SQLite3
find_package(SQLite3 QUIET)
if(NOT SQLite3_FOUND)
    pkg_check_modules(SQLITE3 sqlite3)
endif()

# ncurses (optional)
option(USE_NCURSES "Use ncurses for TUI" OFF)
if(USE_NCURSES)
    find_package(Curses QUIET)
    if(CURSES_FOUND)
        add_definitions(-DHAVE_NCURSES)
        include_directories(${CURSES_INCLUDE_DIRS})
        link_libraries(${CURSES_LIBRARIES})
    endif()
endif()

# Lua (optional)
option(USE_LUA "Use Lua for scripting" OFF)
if(USE_LUA)
    find_package(Lua QUIET)
    if(LUA_FOUND)
        add_definitions(-DHAVE_LUA)
        include_directories(${LUA_INCLUDE_DIR})
        link_libraries(${LUA_LIBRARIES})
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SQLite3_INCLUDE_DIRS}
)

# Source files
set(CORE_SOURCES
    src/core/main.c
    src/core/shell.c
    src/core/config.c
    src/core/log.c
)

set(PARSER_SOURCES
    src/parser/parser.c
)

set(WORKSPACE_SOURCES
    src/workspace/workspace.c
    src/workspace/database.c
)

set(TOOL_SOURCES
    src/tools/tool.c
    src/tools/orchestrator.c
    src/tools/pipeline.c
)

set(GRAPH_SOURCES
    src/graph/graph.c
    src/graph/visualization.c
)

set(SCRIPTING_SOURCES
    src/scripting/lua_engine.c
    src/scripting/dsl.c
)

set(TUI_SOURCES
    src/tui/tui.c
    src/tui/dashboard.c
    src/tui/progress.c
    src/tui/themes.c
)

set(FINDINGS_SOURCES
    src/findings/finding.c
    src/findings/template.c
    src/findings/cvss.c
)

set(REPORTS_SOURCES
    src/reports/report.c
    src/reports/markdown.c
    src/reports/export.c
)

set(PLUGIN_SOURCES
    src/plugins/plugin.c
    src/plugins/loader.c
    src/plugins/api.c
)

set(AI_SOURCES
    src/ai/ai.c
    src/ai/ollama.c
    src/ai/analyzer.c
)

set(UTILS_SOURCES
    src/utils/string_utils.c
    src/utils/file_utils.c
    ${PLATFORM_SOURCES}
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${PARSER_SOURCES}
    ${WORKSPACE_SOURCES}
    ${TOOL_SOURCES}
    ${GRAPH_SOURCES}
    ${SCRIPTING_SOURCES}
    ${TUI_SOURCES}
    ${FINDINGS_SOURCES}
    ${REPORTS_SOURCES}
    ${PLUGIN_SOURCES}
    ${AI_SOURCES}
    ${UTILS_SOURCES}
)

# Create executable
if(WIN32)
    add_executable(gupt WIN32 ${ALL_SOURCES})
else()
    add_executable(gupt ${ALL_SOURCES})
endif()

# Link libraries
target_link_libraries(gupt
    ${SQLite3_LIBRARIES}
    ${PLATFORM_LIBS}
)

# Compiler flags
if(MSVC)
    target_compile_options(gupt PRIVATE /W4)
else()
    target_compile_options(gupt PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(gupt PRIVATE DEBUG)
    target_compile_options(gupt PRIVATE -g)
else()
    target_compile_definitions(gupt PRIVATE NDEBUG)
    target_compile_options(gupt PRIVATE -O2)
endif()

# Install target
install(TARGETS gupt DESTINATION bin)
install(FILES config/gupt.yaml DESTINATION share/gupt)
install(DIRECTORY tools/ DESTINATION share/gupt/tools)
install(DIRECTORY scripts/ DESTINATION share/gupt/scripts)
