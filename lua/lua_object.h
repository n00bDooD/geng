#ifndef LUAOBJ_H
#define LUAOBJ_H
#include <lua.h>

#include "../object.h"

typedef struct {
	const char* name;
	lua_State* thread;
} behaviour;

typedef struct {
	object* o;
} object_ref;

int register_object(lua_State*);

void step_object(object* o, double time_step);

object_ref* luaG_checkobject(lua_State* L, int index);
object_ref* luaG_pushobject(lua_State* L, object*);

#endif /* LUAOBJ_H */

