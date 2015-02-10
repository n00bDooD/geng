#include "lua_physics.h"

#include <lua.h>
#include <lauxlib.h>

#include "lua_scene.h"
#include "lua_colliders.h"
#include "lua_vector.h"
#include "lua_box.h"

#include <chipmunk/chipmunk.h>

#define TYPE_NAME "physics"

static int lua_query_nearest(lua_State* l)
{
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");

	cpNearestPointQueryInfo qi;
	if (cpSpaceNearestPointQueryNearest(
			physics_data,
			*luaG_checkvect(l, 1),
			luaL_checknumber(l, 2),
			CP_ALL_LAYERS,
			CP_NO_GROUP,
			&qi) == NULL) {
		lua_pushnil(l);
		return 1;
	}
	collider* c = luaG_pushcoll(l);
	c->shape = qi.shape;

	cpVect* v = luaG_pushvect(l);
	v->x = qi.p.x;
	v->y = qi.p.y;
	lua_pushnumber(l, qi.d);
	cpVect* v2 = luaG_pushvect(l);
	v2->x = qi.g.x;
	v2->y = qi.g.y;
	return 4;
}

void bbqueryfunc(cpShape* sh, void* data)
{
	lua_State* l = (lua_State*)data;
	// Functions are deleted after calls,
	// so we'll save the function for later.
	lua_pushvalue(l, -1);
	collider* c = luaG_pushcoll(l);
	c->shape = sh;
	lua_pcall(l, 1, 0, 0);
}

static int lua_query_aabb(lua_State* l)
{
	scene* s = get_scene_registry(l);
	cpBB* box = luaG_checkbox(l, 1);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");

	luaL_checktype(l, 2, LUA_TFUNCTION);
	// Func has to be at the top of the stack
	lua_settop(l, 2);
	cpSpaceBBQuery(physics_data, *box,
			CP_ALL_LAYERS,
			CP_NO_GROUP,
			bbqueryfunc, l);
	return 0;
}

void segmentqueryfunc(cpShape* sh, cpFloat t, cpVect n, void* data)
{
	lua_State* l = (lua_State*)data;
	lua_pushvalue(l, -1);
	collider* c = luaG_pushcoll(l);
	c->shape = sh;
	lua_pushnumber(l, t);
	cpVect* v = luaG_pushvect(l);
	*v = n;
	lua_pcall(l, 3, 0, 0);
}

static int lua_query_segment(lua_State* l)
{
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");
	cpVect* start = luaG_checkvect(l, 1);
	cpVect* end = luaG_checkvect(l, 2);

	cpSpaceSegmentQuery(physics_data,
			*start, *end,
			CP_ALL_LAYERS,
			CP_NO_GROUP,
			segmentqueryfunc, l);
	return 0;
}

static int lua_query_segment_first(lua_State* l)
{
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");
	cpVect* start = luaG_checkvect(l, 1);
	cpVect* end = luaG_checkvect(l, 2);

	cpSegmentQueryInfo i;
	if(cpSpaceSegmentQueryFirst(physics_data,
				*start, *end,
				CP_ALL_LAYERS,
				CP_NO_GROUP,
				&i) == NULL) {
		lua_pushnil(l);
		return 1;
	}
	collider* c = luaG_pushcoll(l);
	c->shape = i.shape;
	lua_pushnumber(l, i.t);
	cpVect* v = luaG_pushvect(l);
	*v = i.n;
	return 3;
}

static int lua_set_drag(lua_State* l) {
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");
	cpSpaceSetDamping(physics_data, luaL_checknumber(l, 1));
	return 0;
}

static int lua_get_drag(lua_State* l) {
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");
	lua_pushnumber(l, cpSpaceGetDamping(physics_data));
	return 1;
}


static int lua_set_gravity(lua_State* l) {
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");
	cpSpaceSetGravity(physics_data, *(luaG_checkvect(l, 1)));
	return 0;
}

static int lua_get_gravity(lua_State* l)
{
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");
	cpVect* v = luaG_pushvect(l);
	*v = cpSpaceGetGravity(physics_data);
	return 1;
}

static int lua_reindex_static(lua_State* l)
{
	scene* s = get_scene_registry(l);
	void* physics_data = get_scene_physics(s);
	if (physics_data == NULL) luaL_error(l, "No physics for current scene");

	cpSpaceReindexStatic(physics_data);

	return 0;
}

static const luaL_Reg methods[] = {
	{"nearest", lua_query_nearest},
	{"box", lua_query_aabb},
	{"segment", lua_query_segment},
	{"segment_first", lua_query_segment_first},
	{"drag", lua_get_drag},
	{"set_drag", lua_set_drag},
	{"gravity", lua_get_gravity},
	{"set_gravity", lua_set_gravity},
	{"reindex_static", lua_reindex_static},
	{NULL, NULL}
};

static const luaL_Reg meta_methods[] = {
	{NULL, NULL}
};

int register_physics(lua_State *L)
{
	luaL_register(L, TYPE_NAME, methods);
	lua_pop(L, 1);
	return 0;
}

