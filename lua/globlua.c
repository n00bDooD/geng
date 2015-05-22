#include "globlua.h"
#include "lua_audio.h"
#include "lua_colliders.h"
#include "lua_input.h"
#include "lua_object.h"
#include "lua_renderer.h"
#include "lua_collisionpair.h"
#include "lua_scene.h"
#include "lua_vector.h"
#include "lua_box.h"
#include "lua_physics.h"
#include "lua_copy.h"
#include <stdlib.h>
#include <stdio.h>
#include <lualib.h>

#include "debuglua.h"

void luaG_register_all(lua_State* s, scene* se, inputaxis_data* i, sdl_audio* a, sdl_renderer* r)
{
	register_object(s);
	lua_pop(s, 1);
	register_input(s, i);
	lua_pop(s, 1);
	register_renderer(s, r);
	register_vector(s);
	lua_pop(s, 1);
	register_box(s);
	lua_pop(s, 1);
	register_colliders(s);
	lua_pop(s, 1);
	register_scene(s, se);
	//lua_pop(s, 1);
	register_collpair(s);
	lua_pop(s, 1);
	register_physics(s);
	//lua_pop(s, 1);
	register_audio(s, a);
	lua_pop(s, 1);
}

void dbg_printstack(lua_State* l)
{
	int sz = lua_gettop(l);
	for(int i = sz; i > 0; --i) {
		const char* t = lua_typename(l, lua_type(l, i));
		fprintf(stderr, "%i:[%s]\n", (int)i, t);
	}
}

void dbg_printtable(lua_State* L, int t)
{
	lua_pushnil(L);  /* first key */
	while (lua_next(L, t) != 0) {
  		/* uses 'key' (at index -2) and 'value' (at index -1) */
  		printf("%s - %s\n",
	                  	lua_typename(L, lua_type(L, -2)),
		                lua_typename(L, lua_type(L, -1)));
  		/* removes 'value'; keeps 'key' for next iteration */
  		lua_pop(L, 1);
	}
}

static int lua_error_handler(lua_State* l)
{
	const char* errmesg = lua_tolstring(l, -1, NULL);
	luaL_dostring(l, "debug.debug()");

	lua_settop(l, 0);
	if (errmesg != NULL) {
		lua_pushstring(l, errmesg);
	} else {
		lua_pushnil(l);
	}
	return 1;
}

int luaG_pcall(lua_State* l, int nargs, int nresults)
{
	lua_pushcfunction(l, &lua_error_handler);
	lua_insert(l, 1);
	int ret = lua_pcall(l, nargs, nresults, 1);
	lua_remove(l, 1);
	return ret;
}


/* 
 * Registry key for the engine-specific global state-table.
 * This is done this way because the linker ensures
 * uniqueness of this address, and only addresses can be
 * pushed as light userdata in lua
 */
static const char lua_registry_key = 0;

void luaG_getreg(lua_State* l, const char* regname)
{
	/* Get our special registy table of global 
	 * variables, using the address of a const variable.
	 * This will allow the linker to ensure that this key
	 * is unique to other pointers. */
	lua_pushlightuserdata(l, (void*)&lua_registry_key);
	lua_rawget(l, LUA_REGISTRYINDEX);

	/* Assume it's a table.
	 * Get the key at regname from that table. */
	lua_pushstring(l, regname);
	lua_rawget(l, -2);

	/* Remove the globals table, leaving only the
	 * retrieved value on the stack. */
	lua_remove(l, -2);
}

void luaG_setreg(lua_State* l, const char* regname)
{
	lua_pushlightuserdata(l, (void*)&lua_registry_key);

	/* Get value-table */
	lua_pushlightuserdata(l, (void*)&lua_registry_key);
	lua_rawget(l, LUA_REGISTRYINDEX);

	lua_pushstring(l, regname);
	lua_pushvalue(l, -4);
	/* Value to be set at the top.
	 * Below that, the value name.
	 * At index -3 is the registry table */
	lua_rawset(l, -3);

	/* Value table is now at the top of the stack.
	 * Save it to the lua registry */
	lua_rawset(l, LUA_REGISTRYINDEX);

	lua_pop(l, 1);
}

void luaG_copy_state(lua_State* from, lua_State* to)
{
	/* Get global-value registry table */
	lua_pushlightuserdata(from, (void*)&lua_registry_key);
	lua_rawget(from, LUA_REGISTRYINDEX);
	
	lua_pushlightuserdata(to, (void*)&lua_registry_key);

	/* Do the copy */
	luaExt_copy(from, to);

	/* Set global-value registry table */
	lua_rawset(to, LUA_REGISTRYINDEX);

	/* OK */
	lua_pop(from, 1);
}

void luaG_init_state(lua_State* l)
{
	lua_pushlightuserdata(l, (void*)&lua_registry_key);
	lua_newtable(l);
	lua_rawset(l, LUA_REGISTRYINDEX);

	luaG_register_all(l, NULL, NULL, NULL, NULL);
}

lua_State* luaG_newstate(lua_State* l)
{
	lua_State* s = luaL_newstate();
	if (s == NULL) {
		return NULL;
	}
	luaL_openlibs(s);
	if(l == NULL) {
		luaG_init_state(s);
	} else {
		luaG_copy_state(l, s);
	}
	setup_debug(s);
	return s;
}

