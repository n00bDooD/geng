#ifndef LUA_BOX_H
#define LUA_BOX_H

#include <lua.h>
#include <chipmunk/chipmunk.h>

int register_box(lua_State*);

cpBB* luaG_checkbox(lua_State*, int);
cpBB* luaG_pushbox(lua_State*);

#endif /* LUA_BOX_H */
