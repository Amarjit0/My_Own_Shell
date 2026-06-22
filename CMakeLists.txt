cmake_minimum_required(VERSION 3.20)
project(kalishell VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(LUA REQUIRED lua5.4)
pkg_check_modules(CURSES REQUIRED ncurses)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${SQLITE3_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${CURSES_INCLUDE_DIRS}
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
    src/parser/tokenizer.c
)

set(COMPLETION_SOURCES
    src/completion/completion.c
)

set(WORKSPACE_SOURCES
    src/workspace/workspace.c
    src/workspace/database.c
    src/workspace/target.c
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
    src/utils/platform.c
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${PARSER_SOURCES}
    ${COMPLETION_SOURCES}
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
add_executable(kalishell ${ALL_SOURCES})

# Link libraries
target_link_libraries(kalishell
    ${SQLITE3_LIBRARIES}
    ${LUA_LIBRARIES}
    ${CURSES_LIBRARIES}
    dl
    m
)

# Compiler flags
target_compile_options(kalishell PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -g
)

# Install target
install(TARGETS kalishell DESTINATION bin)
