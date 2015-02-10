#ifndef LUA_COPY_H
#define LUA_COPY_H

#include <lua.h>

/*
 * luaExt_copy
 *
 * Makes a copy of the value at the
 * highest location in the stack of a
 * and pushes it to the stack of b.
 *
 * Returns 0 if the value was successfully copied,
 * and any other value otherwise.
 */
int luaExt_copy(lua_State* a, lua_State* b);

#endif /* LUA_COPY_H */
