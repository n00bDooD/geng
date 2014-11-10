#ifndef LUAOBJ_H
#define LUAOBJ_H
#include <lua.h>

#include "../object.h"

typedef struct {
	object* o;
} object_ref;

int register_object(lua_State*);

object_ref* luaG_checkobject(lua_State* L, int index);
object_ref* luaG_pushobject(lua_State* L);

#endif /* LUAOBJ_H */

