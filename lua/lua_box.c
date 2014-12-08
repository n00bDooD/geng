#include "lua_box.h"

#include "lua_vector.h"

#include <lua.h>
#include <lauxlib.h>

#include <chipmunk/chipmunk.h>

#define TYPE_NAME "box"

cpBB* luaG_checkbox(lua_State* L, int index)
{
	cpBB* o;
	luaL_checktype(L, index, LUA_TUSERDATA);
	o = (cpBB*)luaL_checkudata(L, index, TYPE_NAME);
	if (o == NULL) luaL_typerror(L, index, TYPE_NAME);
	return o;
}

cpBB* luaG_pushbox(lua_State* L)
{
	cpBB* v = (cpBB*)lua_newuserdata(L, sizeof(cpBB));
	luaL_getmetatable(L, TYPE_NAME);
	lua_setmetatable(L, -2);
	return v;
}


static int lua_box_intersects(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpBB* b = luaG_checkbox(l, 2);
	lua_pushboolean(l, cpBBIntersects(*a, *b));
	return 1;
}

static int lua_box_contains(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpBB* b = luaG_checkbox(l, 2);
	lua_pushboolean(l, cpBBContainsBB(*a, *b));
	return 1;
}

static int lua_box_containsvect(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpVect* b = luaG_checkvect(l, 2);
	lua_pushboolean(l, cpBBContainsVect(*a, *b));
	return 1;
}

static int lua_box_center(lua_State* l)
{
	cpBB* b = luaG_checkbox(l, 1);
	cpVect* v = luaG_pushvect(l);
	*v = cpBBCenter(*b);
	return 1;
}

static int lua_box_area(lua_State* l)
{
	cpBB* b = luaG_checkbox(l, 1);
	lua_pushnumber(l, cpBBArea(*b));
	return 1;
}

static int lua_box_mergedarea(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpBB* b = luaG_checkbox(l, 2);
	lua_pushnumber(l, cpBBMergedArea(*a,*b));
	return 1;
}

static int lua_box_merge(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpBB* b = luaG_checkbox(l, 2);
	cpBB* ret = luaG_pushbox(l);
	*ret = cpBBMerge(*a, *b);
	return 1;
}

static int lua_box_expand(lua_State* l)
{
	cpBB* a = luaG_checkbox(l, 1);
	cpVect* b = luaG_checkvect(l, 2);
	cpBB* ret = luaG_pushbox(l);
	*ret = cpBBExpand(*a, *b);
	return 1;
}

static int lua_box_segment_query(lua_State* l)
{
	cpBB* b = luaG_checkbox(l, 1);
	cpVect* f = luaG_checkvect(l, 2);
	cpVect* t = luaG_checkvect(l, 3);

	lua_pushnumber(l, cpBBSegmentQuery(*b, *f, *t));
	return 1;
}

static int lua_box_intersectssegment(lua_State* l)
{
	cpBB* b = luaG_checkbox(l, 1);
	cpVect* f = luaG_checkvect(l, 2);
	cpVect* t = luaG_checkvect(l, 3);

	lua_pushboolean(l, cpBBIntersectsSegment(*b, *f, *t));
	return 1;
}

static int lua_box_new(lua_State* l)
{
	double a = luaL_checknumber(l, 1);
	double b = luaL_checknumber(l, 2);
	double c = luaL_checknumber(l, 3);
	double d = luaL_checknumber(l, 4);
	cpBB* n = luaG_pushbox(l);
	*n = cpBBNew(a, b, c, d);
	return 1;
}

static const luaL_reg methods[] = {
	{"new", lua_box_new},
	{"intersects", lua_box_intersects},
	{"intersects_segment", lua_box_intersectssegment},
	{"contains", lua_box_contains},
	{"contains_vect", lua_box_containsvect},
	{"expand", lua_box_expand},
	{"merge", lua_box_merge},
	{"center", lua_box_center},
	{"area", lua_box_area},
	{"area_merged", lua_box_mergedarea},
	{"segment_query", lua_box_segment_query},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	{NULL, NULL}
};

int register_box(lua_State *L)
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
