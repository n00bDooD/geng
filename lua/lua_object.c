#define _DEFAULT_SOURCE
#include "lua_object.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>

#include <string.h>

#include "../scene.h"

#include "lua_vector.h"

#define TYPE_NAME "object"


void add_behaviour(lua_State* l, object* o, const char* name)
{
	behaviour* obj_threads = (behaviour*)o->tag;
	size_t num_behaviours = 0;
	if(obj_threads != NULL) {
		while(obj_threads[num_behaviours++].name != NULL) {
			const char* behn = obj_threads[num_behaviours++].name;
			if(name != NULL && strcmp(behn, name) == 0) {
				luaL_error(l, "Object already has this behaviour");
			}
		}
	} else {
		num_behaviours = 1;
	}

	behaviour* n = (behaviour*)realloc(obj_threads,
			(num_behaviours+1) * sizeof(behaviour));
	if (n == NULL) luaL_error(l, "Memory allocation error.");
	o->tag = n; obj_threads = n;
	obj_threads[num_behaviours].name = NULL;
	obj_threads[num_behaviours].thread = NULL;

	obj_threads[num_behaviours-1].name = strdup(name);
	obj_threads[num_behaviours-1].thread = lua_newthread(l);
	lua_State* t = obj_threads[num_behaviours-1].thread;

	luaL_ref(l, LUA_REGISTRYINDEX);


	char* key = (char*)calloc(256, sizeof(char));
	strcat(key, "geng.behaviours.");
	strcat(key, name);

	lua_pushstring(t, key);
	free(key);
	lua_rawget(t, LUA_REGISTRYINDEX);
	if(lua_isnil(t, -1)) {
		// Reset thread state
		lua_pop(t, -1);
		luaL_error(l, "Unknown behaviour.");
	}
	int run_result = lua_resume(t, 0);
	switch(run_result) {
		case LUA_YIELD: {
			// OK
			return;
			}
		case 0:
			fprintf(stderr, "Behaviour init did not yield!\n");
			return;
		case LUA_ERRRUN:
		case LUA_ERRMEM:
		case LUA_ERRERR:{
			const char* error = lua_tolstring(t, -1, NULL);
			luaL_error(l, error);
			}
	}
	return;
}



object_ref* luaG_checkobject(lua_State* L, int index)
{
	object_ref* o;
	luaL_checktype(L, index, LUA_TUSERDATA);
	o = (object_ref*)luaL_checkudata(L, index, TYPE_NAME);
	if (o == NULL) luaL_typerror(L, index, TYPE_NAME);
	return o;
}

object_ref* luaG_pushobject(lua_State *L, object* obj)
{
	object_ref* o = (object_ref*)lua_newuserdata(L, sizeof(object_ref));
	luaL_getmetatable(L, TYPE_NAME);
	lua_setmetatable(L, -2);
	o->o = obj;
	return o;
}

static int lua_add_behaviour(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	const char* name = luaL_checklstring(l, 2, NULL);
	if(name == NULL) luaL_error(l, "Valid behaviour name required");
	add_behaviour(l, o->o, name);
	return 0;
}

static int lua_object_equals(lua_State* l)
{
	object_ref* a = luaG_checkobject(l, 1);
	object_ref* b = luaG_checkobject(l, 2);

	lua_pushboolean(l, a->o == b->o);
	return 1;
}

static int lua_object_gc(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	return 0;
}

static int lua_object_tostring(lua_State *L)
{
	object_ref* o = luaG_checkobject(L, 1);
	lua_pushstring(L, o->o->name);
	return 1;
}

static int lua_object_position(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpVect* v = luaG_pushvect(l);
	*v = get_object_pos(o->o);
	return 1;
}

static int lua_object_setposition(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);

	double x, y;
	if(lua_isnumber(l, 2)) {
		x = luaL_checknumber(l, 2);
		y = luaL_checknumber(l, 3);
	} else {
		cpVect* v = luaG_checkvect(l, 2);
		x = v->x;
		y = v->y;
	}

	set_object_pos(o->o, cpv(x, y));

	return 0;
}

static int lua_object_mass(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_mass(o->o));
	return 1;
}

static int lua_object_moment(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_moment(o->o));
	return 1;
}

static int lua_object_torque(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_torque(o->o));
	return 1;
}

static int lua_object_angvel(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_angular_velocity(o->o));
	return 1;
}

static int lua_object_angvel_limit(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_angular_velocity_limit(o->o));
	return 1;
}

static int lua_object_vel_limit(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_velocity_limit(o->o));
	return 1;
}

static int lua_object_angle(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushnumber(l, get_object_angle(o->o));
	return 1;
}

static int lua_object_setsprite(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	size_t s = luaL_checkinteger(l, 2);

	o->o->sprite = s;
	return 0;
}

static int lua_object_set_static(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	disable_object_physics(o->o);
	return 0;
}

static int lua_object_set_physics(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	double mass = luaL_checknumber(l, 2);
	double moment = luaL_checknumber(l, 3);

	enable_object_physics(o->o, mass, moment);
	return 0;
}

static int lua_object_worldpos2local(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpVect* v = luaG_checkvect(l, 2);
	cpVect* res = luaG_pushvect(l);
	*res = object_convert_world2localpos(o->o, *v);
	return 1;
}

static int lua_object_localpos2world(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpVect* v = luaG_checkvect(l, 2);
	cpVect* res = luaG_pushvect(l);
	*res = object_convert_local2worldpos(o->o, *v);
	return 1;
}

static int lua_object_reset_forces(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	object_reset_forces(o->o);
	return 0;
}

static int lua_object_apply_force(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpVect* f = luaG_checkvect(l, 2);
	const cpVect* v = &cpvzero;
	if(lua_gettop(l) > 2) {
		v = luaG_checkvect(l, 3);
	} 
	object_apply_force(o->o, *f, *v);
	return 0;
}

static int lua_object_apply_impulse(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpVect* f = luaG_checkvect(l, 2);
	const cpVect* v = &cpvzero;
	if(lua_gettop(l) > 2) {
		v = luaG_checkvect(l, 3);
	} 
	object_apply_impulse(o->o, *f, *v);
	return 0;
}

static const luaL_reg methods[] = {
	{"pos", lua_object_position},
	{"set_pos", lua_object_setposition},
	{"angle", lua_object_angle},
	{"set_sprite", lua_object_setsprite},
	{"disable_physics", lua_object_set_static},
	{"enable_physics", lua_object_set_physics},
	{"apply_force", lua_object_apply_force},
	{"apply_impulse", lua_object_apply_impulse},
	{"reset_forces", lua_object_reset_forces},
	{"local2worldpos", lua_object_localpos2world},
	{"world2localpos", lua_object_worldpos2local},
	{"torque", lua_object_torque},
	{"mass", lua_object_mass},
	{"moment", lua_object_moment},
	{"ang_vel", lua_object_angvel},
	{"vel_limit", lua_object_vel_limit},
	{"angular_vel_limit", lua_object_angvel_limit},
	{"add_behaviour", lua_add_behaviour},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	{"__tostring", lua_object_tostring},
	{"__eq", lua_object_equals},
	{"__gc", lua_object_gc},
	{NULL, NULL}
};

int register_object(lua_State *L)
{
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	luaL_openlib(L, 0, meta_methods, 0); /* dill metatable */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3); 		/* duplicate methods table */
	lua_rawset(L, -3);		/* metatable.__index = methods */
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);		/* duplicate methods table */
	lua_rawset(L, -3);		/* hide metatable:
					   metatabme.__metatable = methods */

	lua_pop(L, 1); 			/* drop metatable */
	return 1;			/* leave methods on stack */
}

void run_update_method(object* o, lua_State* l, const char* bname, double time_step)
{
	char* thing = NULL;
	switch(lua_status(l)) {
		case 0:
			thing = "ok"; break;
		case LUA_YIELD:
			thing = "yielded"; break;
		case LUA_ERRMEM:
			thing = "errmem"; break;
		case LUA_ERRRUN:
			thing = "errun"; break;
		case LUA_ERRERR:
			thing = "errerrerrerrr"; break;
		default:
			thing = "dunno"; break;
	}
	lua_getglobal(l, "update");
	luaG_pushobject(l, o);
	lua_pushnumber(l, time_step);
	int result = lua_pcall(l, 2, 0, 0);
	if(result != 0) {
		const char* err = lua_tolstring(l, 1, NULL);
		if(err == NULL) err = "Lua error in update";
		fprintf(stderr, "Error in update for behaviour '%s': %s\n", bname, err);
	}
}

void step_object(object* o, double time_step)
{
	size_t i = 0;
	behaviour* bs = (behaviour*)o->tag;
	if(bs == NULL) return;
	while(bs[i++].name != NULL) {
		lua_State* t = bs[i-1].thread;

		run_update_method(o, t, bs[i-1].name, time_step);
	}
}
