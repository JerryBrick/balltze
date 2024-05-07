// SPDX-License-Identifier: GPL-3.0-only

#ifndef BALLTZE__PLUGINS__LUA__HELPERS__FUNCTION_TABLE_HPP
#define BALLTZE__PLUGINS__LUA__HELPERS__FUNCTION_TABLE_HPP

#include <lua.hpp>

namespace Balltze::Plugins::Lua {
    inline void create_functions_table(lua_State *state, const char *name, const luaL_Reg *functions) noexcept {
        lua_pushstring(state, name);
        luaL_newlibtable(state, functions);
        luaL_setfuncs(state, functions, 0);
        lua_settable(state, -3);
    }
}

#endif
