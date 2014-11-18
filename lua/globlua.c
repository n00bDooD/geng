#include "globlua.h"
#include "lua_colliders.h"
#include "lua_input.h"
#include "lua_object.h"
#include "lua_renderer.h"
#include "lua_scene.h"
#include "lua_vector.h"

void luaG_register_all(lua_State* s, scene* se, inputaxis_data* i)
{
	register_object(s);
	lua_pop(s, 1);
	register_input(s, i);
	lua_pop(s, 1);
	register_vector(s);
	lua_pop(s, 1);
	register_colliders(s);
	lua_pop(s, 1);
	register_scene(s, se);
	lua_pop(s, 1);
}
