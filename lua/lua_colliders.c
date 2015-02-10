#include "lua_colliders.h"
#include "lua_object.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../scene.h"

#include <stdbool.h>

#define TYPE_NAME "collider"

collider* luaG_checkcoll(lua_State* L, int index)
{
	collider* o;
	luaL_checktype(L, index, LUA_TUSERDATA);
	o = (collider*)luaL_checkudata(L, index, TYPE_NAME);
	if (o == NULL) luaL_typerror(L, index, TYPE_NAME);
	return o;
}

collider* luaG_pushcoll(lua_State *L)
{
	collider* o = (collider*)lua_newuserdata(L, sizeof(collider));
	luaL_getmetatable(L, TYPE_NAME);
	lua_setmetatable(L, -2);
	o->shape = NULL;
	o->type = NONE;
	return o;
}


static int lua_get_issensor(lua_State* l)
{
	collider* c = luaG_checkcoll(l, 1);
	bool res = false;
	if(c->type != NONE && c->shape != NULL)
		res = cpShapeGetSensor(c->shape);
	lua_pushboolean(l, res);
	return 1;
}

static int lua_set_issensor(lua_State* l)
{
	collider* c = luaG_checkcoll(l, 1);
	if(c->type == NONE || c->shape == NULL)
		luaL_error(l, "Cannot set sensor on a collider without a type.");
	cpShapeSetSensor(c->shape, lua_toboolean(l, 2));
	return 0;
}


static int lua_get_elasticity(lua_State* l)
{
	collider* c = luaG_checkcoll(l, 1);
	if(c->type == NONE || c->shape == NULL)
		luaL_error(l, "Cannot get elasticity on invalid collider.");
	lua_pushnumber(l, cpShapeGetElasticity(c->shape));
	return 1;
}

static int lua_set_elasticity(lua_State* l)
{
	collider* c = luaG_checkcoll(l, 1);
	if(c->type == NONE || c->shape == NULL)
		luaL_error(l, "Cannot set elasticity on invalid collider.");
	double val = luaL_checknumber(l, 2);
	cpShapeSetElasticity(c->shape, val);
	return 0;
}


static int lua_get_friction(lua_State* l)
{
	collider* c = luaG_checkcoll(l, 1);
	if(c->type == NONE || c->shape == NULL)
		luaL_error(l, "Cannot get friction on invalid collider.");
	lua_pushnumber(l, cpShapeGetFriction(c->shape));
	return 1;
}

static int lua_set_friction(lua_State* l)
{
	collider* c = luaG_checkcoll(l, 1);
	if(c->type == NONE || c->shape == NULL)
		luaL_error(l, "Cannot set friction on invalid collider.");
	double val = luaL_checknumber(l, 2);
	cpShapeSetFriction(c->shape, val);
	return 0;
}


static const luaL_reg methods [] = {
	{"issensor", lua_get_issensor},
	{"set_issensor", lua_set_issensor},
	{"elasticity", lua_get_elasticity},
	{"set_elasticity", lua_set_elasticity},
	{"friction", lua_get_friction},
	{"set_friction", lua_set_friction},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	{NULL, NULL}
};

int register_colliders(lua_State* L)
{
	luaL_openlib(L, TYPE_NAME, methods, 0);
	luaL_newmetatable(L, TYPE_NAME);
	luaL_openlib(L, 0, meta_methods, 0);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 1); 
	return 1;
}

collider* object_add_circle(object* o, double radius, cpVect offset)
{
	collider* c = (collider*)calloc(1, sizeof(collider));
	c->type = CIRCLE;
	c->shape = cpCircleShapeNew(o->physics, radius, offset);
	cpSpaceAddShape(get_scene_physics(o->parent), c->shape);
	cpBodySetMoment(o->physics, cpMomentForCircle(cpBodyGetMass(o->physics), 0, radius, offset));
	return c;
}

collider* object_add_segment(object* o, cpVect a, cpVect b, double radius)
{
	collider* c = (collider*)calloc(1, sizeof(collider));
	c->type = LINE;
	c->shape = cpSegmentShapeNew(o->physics, a, b, radius);
	cpSpaceAddShape(get_scene_physics(o->parent), c->shape);
	return c;
}

collider* object_add_box(object* o, double w, double h)
{
	collider* c = (collider*)calloc(1, sizeof(collider));
	c->type = POLY;
	c->shape = cpBoxShapeNew(o->physics, w, h);
	cpSpaceAddShape(get_scene_physics(o->parent), c->shape);
	return c;
}

collider* object_add_poly(object* o, cpVect* verts, int num_verts, cpVect offset)
{
	collider* c = (collider*)calloc(1, sizeof(collider));
	c->type = POLY;
	c->shape = cpPolyShapeNew(o->physics, num_verts, verts, offset);
	cpSpaceAddShape(get_scene_physics(o->parent), c->shape);
	return c;
}

cpVect polycoll_get_centroid(cpVect* verts, int num_verts)
{
	return cpCentroidForPoly(num_verts, verts);
}

int polycoll_validate(cpVect* verts, int num_verts)
{
	return cpPolyValidate(verts, num_verts);
}

void polycoll_center(cpVect* verts, int num_verts)
{
	cpRecenterPoly(num_verts, verts);
}
