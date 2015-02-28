#define _DEFAULT_SOURCE
#include "lua_object.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>

#include <string.h>

#include "../scene.h"
#include "../global.h"

#include "../cbehaviour.h"

#include "globlua.h"

#include "lua_vector.h"
#include "lua_colliders.h"
#include "lua_collisionpair.h"
#include "lua_scene.h"
#include "lua_input.h"
#include "lua_audio.h"

// lua tools
#include "lua_copy.h"

#define TYPE_NAME "object"

void add_behaviour(lua_State* l, object* o, const char* name);
void run_update_method(object* o, lua_State* l, const char* bname, double time_step);


void body_foreach_shape(cpBody* b, cpShape* s, void* data);
void body_foreach_collisionpair(cpBody* b, cpArbiter* a, void* data);
//void body_foreach_constraint(cpBody* b, cpConstraint* s, void* data);

void add_behaviour(lua_State* l, object* o, const char* name)
{
	behaviour* obj_threads = (behaviour*)o->tag;
	size_t num_behaviours = 0;
	if(obj_threads != NULL) {
		while(obj_threads[num_behaviours].name != NULL) {
			const char* behn = obj_threads[num_behaviours++].name;
			if(name != NULL && strcmp(behn, name) == 0) {
				luaL_error(l, "Object already has this behaviour");
			}
		}
		num_behaviours++;
	} else {
		num_behaviours = 1;
	}

	behaviour* n = (behaviour*)realloc(obj_threads,
			(num_behaviours+1) * sizeof(behaviour));
	if (n == NULL) {
		luaL_error(l, "Memory allocation error.");
	}
	o->tag = n; obj_threads = n;
	obj_threads[num_behaviours].name = NULL;
	obj_threads[num_behaviours].content.thread = NULL;

	// See if we are adding a cbehaviour
	luaG_getreg(l, "behaviours");
	lua_pushstring(l, name);
	lua_rawget(l, -2);
	int type = lua_type(l, -1);


	if (type == LUA_TNIL) {
		lua_pop(l, 2);
		luaL_error(l, "Unknown behaviour.");
	} else if (type == LUA_TLIGHTUSERDATA) {
		obj_threads[num_behaviours-1].name = strdup(name);
		obj_threads[num_behaviours-1].script_behaviour = false;
		obj_threads[num_behaviours-1].content.beh = lua_touserdata(l, -1);
		if(obj_threads[num_behaviours-1].content.beh == NULL) {
			luaL_error(l, "NULL cbehaviour!!");
		}
		lua_pop(l, 2);
		call_create(obj_threads[num_behaviours-1].content.beh, o, l);
	} else {
		lua_pop(l, 2);
		obj_threads[num_behaviours-1].name = strdup(name);
		obj_threads[num_behaviours-1].script_behaviour = true;
		obj_threads[num_behaviours-1].content.thread = luaG_newstate(l);
		lua_State* t = obj_threads[num_behaviours-1].content.thread;
		luaL_openlibs(t);
		luaG_register_all(t,
				get_scene_registry(l),
				get_input_registry(l),
				get_audio_registry(l)
				);

		luaG_getreg(l, "behaviours");
		if(lua_isnil(l, -1)) {
			luaL_error(l, "No behaviour table");
		}

		luaExt_copy(l, t);
		luaG_setreg(t, "behaviours");
		lua_pop(l, 1);

		luaG_getreg(l, "prefabs");
		if(lua_isnil(l, -1)) {
			luaL_error(l, "No prefab table");
		}

		luaExt_copy(l, t);
		luaG_setreg(t, "prefabs");
		lua_pop(l, 1);

		luaG_getreg(t, "behaviours");

		lua_pushstring(t, name);
		lua_rawget(t, -2);
		if(lua_isnil(t, -1)) {
			// Reset thread state
			lua_pop(t, 2);
			luaL_error(l, "Unknown behaviour.");
		}

		// Argument copying
		int num_args = lua_gettop(l);
		lua_createtable(t, num_args, 0);
		for(int argi = num_args; argi > 0; argi--) {
			luaExt_copy(l, t);
			lua_pop(l, 1);
			lua_rawseti(t, -2, argi);
		}
		lua_setglobal(t, "args");

		int run_result = lua_pcall(t, 0, 0, 0);
		switch(run_result) {
			case 0:
			case LUA_YIELD: {
				// OK
				return;
				}
			case LUA_ERRRUN:
			case LUA_ERRMEM:
			case LUA_ERRERR:{
				const char* error = lua_tolstring(t, -1, NULL);
				luaL_error(l, error);
				}
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

static int lua_object_delete(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	o->o->flags |= OBJ_DELETED;
	return 0;
}

static int lua_add_behaviour(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	char* name = strdup(luaL_checklstring(l, 2, NULL));
	if(name == NULL) {
		luaL_error(l, "Valid behaviour name required");
	}
	lua_remove(l, 1);
	lua_remove(l, 1);
	add_behaviour(l, o->o, name);
	free(name);
	return 0;
}

static int lua_object_equals(lua_State* l)
{
	object_ref* a = luaG_checkobject(l, 1);
	object_ref* b = luaG_checkobject(l, 2);

	lua_pushboolean(l, a->o == b->o);
	return 1;
}

static int lua_object_tostring(lua_State *L)
{
	object_ref* o = luaG_checkobject(L, 1);
	lua_pushstring(L, o->o->name);
	return 1;
}

static int lua_object_set_name(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	if (o->o->name != NULL) {
		free(o->o->name);
		o->o->name = NULL;
	}
	const char* name = luaL_checklstring(l, 2, NULL);
	if (name != NULL) {
		o->o->name = strdup(name);
	}
	return 0;
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

static int lua_object_velocity(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpVect* res = luaG_pushvect(l);
	*res = get_object_velocity(o->o);
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

static int lua_object_set_angvel_limit(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	cpBodySetAngVelLimit(o->o->physics, luaL_checknumber(l, 2));
	return 0;
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

static int lua_object_setangle(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	double a = luaL_checknumber(l, 2);
	set_object_angle(o->o, a);
	return 0;
}

static int lua_object_setsprite(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_Integer s = luaL_checkinteger(l, 2);
	if (s < 0) luaL_error(l, "Sprite cannot be negative");

	o->o->sprite = (size_t)s;
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

static int lua_object_add_circlecoll(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	double radius = luaL_checknumber(l, 2);
	const cpVect* offset = &cpvzero;
	if(lua_gettop(l) > 2) {
		offset = luaG_checkvect(l, 3);
	}
	collider* c = object_add_circle(o->o, radius, *offset);
	collider* ret = luaG_pushcoll(l);
	memcpy(ret, c, sizeof(collider));
	free(c);
	return 1;
}

static int lua_object_add_boxcoll(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	double w = luaL_checknumber(l, 2);
	double h = luaL_checknumber(l, 3);
	collider* c = object_add_box(o->o, w, h);
	collider* ret = luaG_pushcoll(l);
	memcpy(ret, c, sizeof(collider));
	free(c);
	return 1;
}

static int lua_object_set_fliph(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	if(lua_toboolean(l, 2) != 0) {
		o->o->flags |= OBJ_FLIPHOR;
	} else {
		o->o->flags &= ~OBJ_FLIPHOR;
	}
	return 0;
}

static int lua_object_set_flipv(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	if(lua_toboolean(l, 2) != 0) {
		o->o->flags |= OBJ_FLIPVERT;
	} else {
		o->o->flags &= ~OBJ_FLIPVERT;
	}
	return 0;
}

static int lua_object_get_fliph(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushboolean(l, o->o->flags & OBJ_FLIPHOR);
	return 1;
}

static int lua_object_get_flipv(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_pushboolean(l, o->o->flags & OBJ_FLIPVERT);
	return 1;
}

static int lua_object_send_message(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	const char* name = luaL_checklstring(l, 2, NULL);
	if (name == NULL) luaL_error(l, "Invalid name");
	// Only one argument
	lua_settop(l, 3);

	behaviour* obj_threads = o->o->tag;
	if (obj_threads == NULL) return 0;
	size_t num_behaviours = 0;
	while(obj_threads[num_behaviours].name != NULL) {
		const char* behn = obj_threads[num_behaviours++].name;
		if(strcmp(behn, name) == 0) {
			lua_State* r = obj_threads[num_behaviours-1].content.thread;
			lua_getglobal(r, "receive");
			if (lua_isnil(r, -1)) {
				lua_pop(r, 1);
				return 0;
			}
			luaG_pushobject(r, o->o);
			luaExt_copy(l, r);
			int result = lua_pcall(r, 2, 0, 0);
			plua_error(r, result, "receive");
			return 0;
		}
	}
	return 0;
}

static int lua_object_foreach_shape(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_remove(l, 1);
	luaL_checktype(l, 1, LUA_TFUNCTION);

	cpBodyEachShape(o->o->physics, &body_foreach_shape, l);
	return 0;
}

static int lua_object_foreach_collisionpair(lua_State* l)
{
	object_ref* o = luaG_checkobject(l, 1);
	lua_remove(l, 1);
	luaL_checktype(l, 1, LUA_TFUNCTION);

	cpBodyEachArbiter(o->o->physics, &body_foreach_collisionpair, l);
	return 0;
}

static const luaL_reg methods[] = {
	{"pos", lua_object_position},
	{"set_pos", lua_object_setposition},
	{"angle", lua_object_angle},
	{"set_angle", lua_object_setangle},
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
	{"velocity", lua_object_velocity},
	{"ang_vel", lua_object_angvel},
	{"vel_limit", lua_object_vel_limit},
	{"angular_vel_limit", lua_object_angvel_limit},
	{"add_behaviour", lua_add_behaviour},
	{"add_circle_collider", lua_object_add_circlecoll},
	{"add_box_collider", lua_object_add_boxcoll},
	{"delete", lua_object_delete},
	{"set_name", lua_object_set_name},
	{"name", lua_object_tostring},
	{"flipv", lua_object_get_flipv},
	{"fliph", lua_object_get_fliph},
	{"set_flipv", lua_object_set_flipv},
	{"set_fliph", lua_object_set_fliph},
	{"send_message", lua_object_send_message},
	{"set_ang_vel_limit", lua_object_set_angvel_limit},

	{"foreach_collider", lua_object_foreach_shape},
	{"foreach_collision", lua_object_foreach_collisionpair},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	//{"__tostring", lua_object_tostring},
	{"__eq", lua_object_equals},
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
	lua_getglobal(l, "scene_update");
	if (lua_isnil(l, -1)) {
		lua_pop(l, 1);
		return;
	}
	luaG_pushobject(l, o);
	lua_pushnumber(l, time_step);
	int result = lua_pcall(l, 2, LUA_MULTRET, 0);
	plua_error(l, result, bname);
}

void step_object(object* o, double time_step)
{
	size_t i = 0;
	behaviour* bs = (behaviour*)o->tag;
	if(bs == NULL) return;
	while(bs[i++].name != NULL) {
		if (bs[i-1].script_behaviour) {
			lua_State* t = bs[i-1].content.thread;

			run_update_method(o, t, bs[i-1].name, time_step);
		} else {
			// Call C behaviour
			call_update(bs[i-1].content.beh, o, time_step);
		}
	}
}

void body_foreach_shape(cpBody* b, cpShape* s, void* data)
{
	lua_State* l = data;

	/* Duplicate function */
	lua_pushvalue(l, 1);

	// Move it to just below the args
	lua_insert(l, 2);

	// Drag up object and collider 
	// and move them just above the
	// function.
	luaG_pushobject(l, cpBodyGetUserData(b));

	collider* c = luaG_pushcoll(l);
	c->shape = s;
	c->type = UNKNOWN;

	lua_insert(l, 3);
	lua_insert(l, 3);

	// Call it. Number of arguments = stack position
	// minus the original function & the function to
	// be called.
	int result = lua_pcall(l, lua_gettop(l) -2, 0, 0);
	plua_error(l, result, "foreach_collider");
}

void body_foreach_collisionpair(cpBody* b, cpArbiter* a, void* data)
{
	UNUSED(b);
	lua_State* l = data;

	/* Duplicate function */
	lua_pushvalue(l, 1);

	// Move it to just below the args
	lua_insert(l, 2);

	// Drag up collision pair and move it just above the
	// function.
	collision_pair* p = luaG_pushcollpair(l, a);
	p->current = COLL_NONE;

	lua_insert(l, 3);

	// Call it. Number of arguments = stack position
	// minus the original function & the function to
	// be called.
	int result = lua_pcall(l, lua_gettop(l) -2, 0, 0);
	plua_error(l, result, "foreach_collisionpair");
}
