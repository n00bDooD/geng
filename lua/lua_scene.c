#define _DEFAULT_SOURCE
#include "lua_scene.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define TYPE_NAME "scene"
#define REGISTRY_KEY "geng.scene"

#include "../global.h"

#include "../game.h"
#include "../scene.h"
#include "globlua.h"
#include "lua_object.h"
#include "lua_copy.h"
#include "lua_input.h"
#include "lua_audio.h"
#include "lua_renderer.h"

#include <stdbool.h>
#include <string.h>
#include <libgen.h>

#include "../cbehaviours/call_logger.h"
#include "../cbehaviours/animation.h"

void set_scene_registry(lua_State* l, scene* s);
void reload_obj_behaviour(object* o, const char* name, lua_State* l);
void reload_behaviour(scene* s, const char* name, lua_State* l);
object* create_prefab(lua_State* l, scene* s, const char* name);

object* create_prefab(lua_State* l, scene* s, const char* name)
{
	UNUSED(s);
	int num_args = lua_gettop(l);

	/* Get prefab table */
	luaG_getreg(l, "prefabs");
	if(lua_isnil(l, -1)) {
		luaL_error(l, "Unknown prefab");
	}

	lua_pushstring(l, name);
	lua_rawget(l, -2);
	if(lua_isnil(l, -1)) {
		luaL_error(l, "Unknown prefab");
	}
	if(num_args > 0) {
		/* move function to space below args */
		lua_insert(l, 1);
		/* move the prefab-table to the lowest spot */
		lua_insert(l, 1);
	}
	lua_createtable(l, num_args, 0);
	lua_insert(l, 3);
	for(int i = 0; i < num_args; ++i){
		lua_rawseti(l, 3, num_args - i);
	}
	lua_setglobal(l, "prefab_args");
	int run_result = luaG_pcall(l, 0, 1);
	switch(run_result) {
		case 0: {
			// OK
			object_ref* res = luaG_checkobject(l, -1);
			if(res == NULL) return NULL;
			return res->o;
			}
		case LUA_ERRERR:
		case LUA_ERRMEM:
		case LUA_ERRRUN:
			{
			const char* error = lua_tolstring(l, -1, NULL);
			luaL_error(l, error);
			}
	}
	return NULL;
}

scene* luaG_checkscene(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	scene* s = NULL;
	s = (scene*)luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

scene* get_scene_registry(lua_State* l)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_rawget(l, LUA_REGISTRYINDEX);
	scene* ret = (scene*)lua_touserdata(l, -1);
	if(ret == NULL) {
		luaL_error(l, "No current scene");
	}

	/* Restore stack */
	lua_pop(l, 1);
	return ret;
}

void set_scene_registry(lua_State* l, scene* s)
{
	lua_pushstring(l, REGISTRY_KEY);
	lua_pushlightuserdata(l, s);
	/* registry['REGISTRY_KEY'] = s */
	lua_rawset(l, LUA_REGISTRYINDEX);
}

static int lua_load_prefab(lua_State* l)
{
	const char* filename = luaL_checklstring(l, 1, NULL);
	if(filename == NULL) {
		luaL_error(l, "Valid name to prefab file required.");
	}
	luaL_checktype(l, 2, LUA_TFUNCTION);
	char* tmp = strdup(filename);
	if(tmp == NULL) {
		luaL_error(l, "Memory allocation error");
	}

	/* Push prefab table on stack */
	luaG_getreg(l, "prefabs");
	if(lua_isnil(l, -1)) {
		lua_newtable(l);
	}
	lua_pushstring(l, tmp);
	free(tmp);

	/* Move the function in stack position 2 (function arg) 
	 * to the top of the stack before setting the table*/
	lua_pushvalue(l, 2);

	/* OK. Set the key prefabname to the loaded string,
	 * in the table at index -3 (prefab-table).
	 * Then, save prefab table in registry. */
	lua_rawset(l, -3);
	luaG_setreg(l, "prefabs");
	return 0;
}

void reload_obj_behaviour(object* o, const char* name, lua_State* l)
{
	behaviour* obj_threads = o->tag;
	if (obj_threads == NULL) return;
	size_t num_behaviours = 0;
	while(obj_threads[num_behaviours].name != NULL) {
		if(!obj_threads[num_behaviours].script_behaviour) {
			continue;
		}
		const char* behn = obj_threads[num_behaviours++].name;
		if(strcmp(behn, name) == 0) {
			lua_State* r = obj_threads[num_behaviours-1].content.thread;

			// This behaviour needs to be reloaded
			luaExt_copy(l, r);
			int run_result = luaG_pcall(r, 0, 0);
			switch(run_result) {
				case 0:
				case LUA_YIELD: {
					// OK
					return;
					}
				case LUA_ERRRUN:
				case LUA_ERRMEM:
				case LUA_ERRERR:{
					const char* error = lua_tolstring(r, -1, NULL);
					luaL_error(l, error);
					}
			}
			return;
		}
	}
	return;
}

void reload_behaviour(scene* s, const char* name, lua_State* l)
{
	for(size_t i = 0; i < s->num_objects; ++i){
		object* o = &(s->pool[i]);
		if((o->flags & OBJ_ACTIVE) != 0) {
			reload_obj_behaviour(o, name, l);
		}
	}
}

static int lua_load_behaviour(lua_State* l)
{
	scene* scn = get_scene_registry(l);

	const char* filename = luaL_checklstring(l, 1, NULL);
	if(filename == NULL) {
		luaL_error(l, "Valid name to behaviour file required.");
	}
	if (lua_type(l, 2) == LUA_TLIGHTUSERDATA) {
		// CBehaviour
		// Nothing needs to be done, as this method is
		// completely agnostic.
		// cfunction or no cfunction will be handled per-object
	} else {
		// Regular behaviour
		luaL_checktype(l, 2, LUA_TFUNCTION);
	}
	char* tmp = strdup(filename);
	if(tmp == NULL) {
		luaL_error(l, "Memory allocation error");
	}

	luaG_getreg(l, "behaviours");
	if(lua_isnil(l, -1)) {
		lua_pop(l, 1);
		lua_newtable(l);
	}

	lua_pushstring(l, tmp);
	free(tmp);

	// Check if we need to reload this behaviour in
	// the entire scene
	lua_pushvalue(l, -1);
	lua_rawget(l, -3);
	// Cbehaviours are not reloaded
	bool reloading_function = !lua_isnil(l, -1) && lua_type(l, -1) != LUA_TLIGHTUSERDATA;
	lua_pop(l, 1);
	
	/* Push function-argument to the top of the stack */
	lua_pushvalue(l, 2);

	if (reloading_function) {
		// Copy values for refreshing the 
		// scene with if needed
		lua_pushvalue(l, -2);
		lua_pushvalue(l, -2);
		lua_insert(l, -5);
		lua_insert(l, -5);
	}

	lua_rawset(l, -3);
	luaG_setreg(l, "behaviours");

	// Reload for current scene
	if (reloading_function) {
		reload_behaviour(scn, tmp, l);
	}
	return 0;
}

static int lua_new_object(lua_State* l)
{
	scene* s = get_scene_registry(l);
	object* o = create_object(s);
	luaG_pushobject(l, o);
	return 1;
}

static int lua_spawn_prefab(lua_State* l)
{
	scene* s = get_scene_registry(l);
	const char* name = luaL_checklstring(l, 1, NULL);

	if(name == NULL) {
		luaL_error(l, "Name required");
	}
	lua_remove(l, 1);

	object* o = create_prefab(l, s, name);
	if(o == NULL) {
		luaL_error(l, "Object NULL error");
	}
	luaG_pushobject(l, o);
	return 1;
}

static int lua_load_scene(lua_State* l)
{
	const char* filename = luaL_checklstring(l, 1, NULL);
	if(filename == NULL) {
		luaL_error(l, "Valid name required.");
	}
	luaL_checktype(l, 2, LUA_TFUNCTION);
	char* tmp = strdup(filename);
	if(tmp == NULL) {
		luaL_error(l, "Memory allocation error");
	}

	/* Push prefab table on stack */
	luaG_getreg(l, "scenes");
	if(lua_isnil(l, -1)) {
		lua_pop(l, 1);
		lua_newtable(l);
	}
	lua_pushstring(l, tmp);
	free(tmp);

	/* Move the function in stack position 2 (function arg) 
	 * to the top of the stack before setting the table*/
	lua_pushvalue(l, 2);

	/* OK. Set the key prefabname to the loaded string,
	 * in the table at index -3 (prefab-table).
	 * Then, save prefab table in registry. */
	lua_rawset(l, -3);
	luaG_setreg(l, "scenes");
	return 0;
}

static int lua_set_scene(lua_State* l)
{
	luaG_getreg(l, "game");
	if (lua_isnil(l, -1)) goto no_game;
	game* g = lua_touserdata(l, -1);
	if (g == NULL) goto no_game;
	lua_pop(l, 1);

	const char* scene_name = luaL_checklstring(l, 1, NULL);
	if(scene_name == NULL) {
		luaL_error(l, "Valid name required.");
	}

	luaG_getreg(l, "scenes");
	if (lua_isnil(l, -1)) goto no_scene;

	lua_pushstring(l, scene_name);
	lua_rawget(l, -2);
	if (lua_isnil(l, -1)) goto no_scene;

	luaG_getreg(l, "physics");
	void* physics = lua_touserdata(l, -1);
	luaG_getreg(l, "renderer");
	void* render = lua_touserdata(l, -1);
	lua_pop(l, 2);

	lua_State* nl = luaG_newstate(l);
	scene* news = create_new_scene(nl, 10000, render, physics);

	game_add_scene(g, news);
	free(news);
	news = &(g->scenes[g->num_scenes-1]);

	luaG_register_all(nl, news,
			  get_input_registry(l),
			  get_audio_registry(l),
			  get_renderer_registry(l)
			  );
	register_call_logger(nl);
	register_animation(nl);

	luaExt_copy(l, nl);
	int res = luaG_pcall(nl, 0, 0);
	plua_error(nl, res, "set_scene");

	game_set_current(g, g->num_scenes);

	return 0;
no_scene:
	luaL_error(l, "Unknown scene");
	return 0;
no_game:
	luaL_error(l, "No current game state");
	return 0;
}

static const luaL_Reg methods[] = {
	{"newobject", lua_new_object},
	{"spawn_prefab", lua_spawn_prefab},
	{"load_prefab", lua_load_prefab},
	{"load_behaviour", lua_load_behaviour},
	{"load_scene", lua_load_scene},
	{"set_scene", lua_set_scene},
	{NULL, NULL}
};

/*
static const luaL_Reg meta_methods[] = {
	{NULL, NULL}
};
*/

int register_scene(lua_State *L, scene* s)
{
	if (s != NULL) set_scene_registry(L, s);

	luaL_register(L, TYPE_NAME, methods);
	lua_pop(L, 1);
	return 0;
}

void step_scene(scene* s, double time_step)
{
	for(size_t i = 0; i < s->num_objects; ++i){
		object* o = &(s->pool[i]);
		if((o->flags & OBJ_ACTIVE) != 0) {
			step_object(o, time_step);
		}
	}
}
