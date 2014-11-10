#ifndef LUAOBJ_H
#define LUAOBJ_H
#include <lua.h>

#include "../object.h"

int register_object(lua_State*);

object* luaG_checkobject(lua_State* L, int index);
object* luaG_pushobject(lua_State* L);

#endif /* LUAOBJ_H */

