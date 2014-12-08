#include "globlua.h"
#include "lua_colliders.h"
#include "lua_input.h"
#include "lua_object.h"
#include "lua_renderer.h"
#include "lua_collisionpair.h"
#include "lua_scene.h"
#include "lua_vector.h"
#include "lua_box.h"
#include "lua_physics.h"
#include <stdlib.h>
#include <stdio.h>

void luaG_register_all(lua_State* s, scene* se, inputaxis_data* i)
{
	register_object(s);
	lua_pop(s, 1);
	register_input(s, i);
	lua_pop(s, 1);
	register_vector(s);
	lua_pop(s, 1);
	register_box(s);
	lua_pop(s, 1);
	register_colliders(s);
	lua_pop(s, 1);
	register_scene(s, se);
	lua_pop(s, 1);
	register_collpair(s);
	lua_pop(s, 1);
	register_physics(s);
	lua_pop(s, 1);
}

void dbg_printstack(lua_State* l)
{
	size_t sz = lua_gettop(l);
	for(size_t i = sz; i > 0; --i) {
		const char* t = lua_typename(l, lua_type(l, i));
		fprintf(stderr, "%i:[%s]\n", (int)i, t);
	}
}
