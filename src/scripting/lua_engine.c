#include "gupt/gupt.h"

#ifdef HAVE_LUA
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#endif

extern ks_shell_t shell;

#ifdef HAVE_LUA
static lua_State *L = NULL;
#endif

// Initialize Lua engine
int ks_lua_init(void) {
#ifdef HAVE_LUA
    L = luaL_newstate();
    if (!L) {
        return KS_ERROR_NOMEM;
    }
    
    luaL_openlibs(L);
    
    // Register custom functions
    lua_pushcfunction(L, ks_lua_run_tool);
    lua_setglobal(L, "run");
    
    lua_pushcfunction(L, ks_lua_print);
    lua_setglobal(L, "print");
    
    lua_pushcfunction(L, ks_lua_count_lines);
    lua_setglobal(L, "count_lines");
    
    lua_pushcfunction(L, ks_lua_file_exists);
    lua_setglobal(L, "file_exists");
    
    lua_pushcfunction(L, ks_lua_read_file);
    lua_setglobal(L, "read_file");
    
    lua_pushcfunction(L, ks_lua_write_file);
    lua_setglobal(L, "write_file");
    
    lua_pushcfunction(L, ks_lua_get_target);
    lua_setglobal(L, "get_target");
    
    lua_pushcfunction(L, ks_lua_set_target);
    lua_setglobal(L, "set_target");
    
    lua_pushcfunction(L, ks_lua_add_finding);
    lua_setglobal(L, "add_finding");
    
    lua_pushcfunction(L, ks_lua_add_asset);
    lua_setglobal(L, "add_asset");
    
    lua_pushcfunction(L, ks_lua_show_progress);
    lua_setglobal(L, "show_progress");
    
    KS_LOG_DEBUG("Lua engine initialized");
    return KS_OK;
#else
    KS_LOG_WARN("Lua support not compiled in");
    return KS_OK;
#endif
}

// Cleanup Lua engine
void ks_lua_cleanup(void) {
#ifdef HAVE_LUA
    if (L) {
        lua_close(L);
        L = NULL;
    }
#endif
}

// Execute Lua script string
int ks_lua_execute(const char *script) {
#ifdef HAVE_LUA
    if (!L || !script) {
        return KS_ERROR_INVALID;
    }
    
    int result = luaL_dostring(L, script);
    if (result != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return KS_ERROR;
    }
    
    return KS_OK;
#else
    fprintf(stderr, "Lua support not available\n");
    return KS_ERROR;
#endif
}

// Execute Lua file
int ks_lua_execute_file(const char *filename) {
#ifdef HAVE_LUA
    if (!L || !filename) {
        return KS_ERROR_INVALID;
    }
    
    int result = luaL_dofile(L, filename);
    if (result != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return KS_ERROR;
    }
    
    return KS_OK;
#else
    fprintf(stderr, "Lua support not available\n");
    return KS_ERROR;
#endif
}

#ifdef HAVE_LUA
// Lua function: run(tool_name, args)
static int ks_lua_run_tool(lua_State *L) {
    const char *tool_name = luaL_checkstring(L, 1);
    const char *args = lua_isstring(L, 2) ? lua_tostring(L, 2) : NULL;
    
    char output[65536] = {0};
    int result = ks_tool_execute(tool_name, args, output, sizeof(output));
    
    if (strlen(output) > 0) {
        lua_pushstring(L, output);
    } else {
        lua_pushstring(L, "");
    }
    
    return 1;
}

// Lua function: print(...)
static int ks_lua_print(lua_State *L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        const char *str = lua_tostring(L, i);
        if (str) {
            printf("%s", str);
        }
        if (i < n) {
            printf("\t");
        }
    }
    printf("\n");
    return 0;
}

// Lua function: count_lines(filename)
static int ks_lua_count_lines(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        count++;
    }
    
    fclose(f);
    lua_pushinteger(L, count);
    return 1;
}

// Lua function: file_exists(filename)
static int ks_lua_file_exists(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);
    lua_pushboolean(L, ks_path_exists(filename));
    return 1;
}

// Lua function: read_file(filename)
static int ks_lua_read_file(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);
    
    char *content = ks_file_read(filename);
    if (content) {
        lua_pushstring(L, content);
        free(content);
    } else {
        lua_pushnil(L);
    }
    
    return 1;
}

// Lua function: write_file(filename, content)
static int ks_lua_write_file(lua_State *L) {
    const char *filename = luaL_checkstring(L, 1);
    const char *content = luaL_checkstring(L, 2);
    
    int result = ks_file_write(filename, content);
    lua_pushboolean(L, result == KS_OK);
    return 1;
}

// Lua function: get_target()
static int ks_lua_get_target(lua_State *L) {
    // TODO: Get current target from workspace
    lua_pushstring(L, "");
    return 1;
}

// Lua function: set_target(target)
static int ks_lua_set_target(lua_State *L) {
    const char *target = luaL_checkstring(L, 1);
    // TODO: Set current target in workspace
    printf("[*] Target set to: %s\n", target);
    return 0;
}

// Lua function: add_finding(title, severity, description)
static int ks_lua_add_finding(lua_State *L) {
    const char *title = luaL_checkstring(L, 1);
    const char *severity = luaL_checkstring(L, 2);
    const char *description = lua_isstring(L, 3) ? lua_tostring(L, 3) : "";
    
    // TODO: Add finding to database
    printf("[+] Finding: %s [%s]\n", title, severity);
    
    return 0;
}

// Lua function: add_asset(type, value)
static int ks_lua_add_asset(lua_State *L) {
    const char *type = luaL_checkstring(L, 1);
    const char *value = luaL_checkstring(L, 2);
    
    // TODO: Add asset to database
    printf("[+] Asset: %s = %s\n", type, value);
    
    return 0;
}

// Lua function: show_progress(label, current, total)
static int ks_lua_show_progress(lua_State *L) {
    const char *label = luaL_checkstring(L, 1);
    int current = luaL_checkinteger(L, 2);
    int total = luaL_checkinteger(L, 3);
    
    int percent = (current * 100) / total;
    ks_tui_draw_progress(label, percent);
    
    return 0;
}
#endif
