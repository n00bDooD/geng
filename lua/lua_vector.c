#include "lua_vector.h"
#include "lua_box.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <chipmunk/chipmunk.h>
#include <string.h>

#define TYPE_NAME "vector"

cpVect* luaG_checkvect(lua_State* L, int index)
{
	cpVect* o;
	luaL_checktype(L, index, LUA_TUSERDATA);
	o = (cpVect*)luaL_checkudata(L, index, TYPE_NAME);
	if (o == NULL) luaL_typerror(L, index, TYPE_NAME);
	return o;
}

cpVect* luaG_optvect(lua_State* L, int index, cpVect* nil)
{
	if (lua_type(L, index) == LUA_TUSERDATA) {
		cpVect* o = (cpVect*)luaL_checkudata(L, index, TYPE_NAME);
		if (o != NULL) return o;
	}
	return nil;
}

cpVect* luaG_pushvect(lua_State* L)
{
	cpVect* v = (cpVect*)lua_newuserdata(L, sizeof(cpVect));
	luaL_getmetatable(L, TYPE_NAME);
	lua_setmetatable(L, -2);
	return v;
}

static int lua_vect_new(lua_State* l)
{
	cpFloat x = 0, y = 0;
	x = luaL_checknumber(l, 1);
	y = luaL_checknumber(l, 2);

	cpVect* v = luaG_pushvect(l);
	*v = cpv(x, y);
	return 1;
}

static int lua_vect_zero(lua_State* l)
{
	cpVect* v = luaG_pushvect(l);
	*v = cpvzero;
	return 1;
}



static int lua_vect_x(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	lua_pushnumber(l, v->x);
	return 1;
}

static int lua_vect_y(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	lua_pushnumber(l, v->y);
	return 1;
}

static int lua_vect_sety(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	double y = luaL_checknumber(l, 2);
	v->y = y;
	return 0;
}

static int lua_vect_setx(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	double x = luaL_checknumber(l, 2);
	v->x = x;
	return 0;
}

static int lua_vect_eq(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpBool res = cpveql(*v1, *v2);
	lua_pushboolean(l, res);
	return 1;
}

static int lua_vect_normalize(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);

	cpVect* n = luaG_pushvect(l);
	*n = cpvnormalize(*v);
	return 1;
}

static int lua_vect_neg(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);

	cpVect* n = luaG_pushvect(l);
	*n = cpvneg(*v);
	return 1;
}

static int lua_vect_perp(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);

	cpVect* n = luaG_pushvect(l);
	*n = cpvperp(*v);
	return 1;
}

static int lua_vect_forangle(lua_State* l)
{
	cpFloat a = luaL_checknumber(l, 1);

	cpVect* n = luaG_pushvect(l);
	*n = cpvforangle(a);
	return 1;
}

static int lua_vect_rperp(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);

	cpVect* n = luaG_pushvect(l);
	*n = cpvrperp(*v);
	return 1;
}

static int lua_vect_toangle(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);

	cpFloat res = cpvtoangle(*v1);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_dot(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpFloat res = cpvdot(*v1, *v2);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_cross(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpFloat res = cpvcross(*v1, *v2);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_dist(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpFloat res = cpvdist(*v1, *v2);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_distsq(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpFloat res = cpvdistsq(*v1, *v2);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_length(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);

	cpFloat res = cpvlength(*v1);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_lengthsq(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);

	cpFloat res = cpvlengthsq(*v1);
	lua_pushnumber(l, res);
	return 1;
}

static int lua_vect_lerp(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);
	cpFloat f = luaL_checknumber(l, 3);

	cpVect* v = luaG_pushvect(l);
	*v = cpvlerp(*v1, *v2, f);
	return 1;
}

static int lua_vect_lerpconst(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);
	cpFloat f = luaL_checknumber(l, 3);

	cpVect* v = luaG_pushvect(l);
	*v = cpvlerpconst(*v1, *v2, f);
	return 1;
}

static int lua_vect_slerp(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);
	cpFloat f = luaL_checknumber(l, 3);

	cpVect* v = luaG_pushvect(l);
	*v = cpvslerp(*v1, *v2, f);
	return 1;
}

static int lua_vect_slerpconst(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);
	cpFloat f = luaL_checknumber(l, 3);

	cpVect* v = luaG_pushvect(l);
	*v = cpvlerpconst(*v1, *v2, f);
	return 1;
}

static int lua_vect_near(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);
	cpFloat f = luaL_checknumber(l, 3);

	cpBool b = cpvnear(*v1, *v2, f);
	lua_pushboolean(l, b);
	return 1;
}

static int lua_vect_clamp(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpFloat f = luaL_checknumber(l, 3);

	cpVect* v = luaG_pushvect(l);
	*v = cpvclamp(*v1, f);
	return 1;
}

static int lua_vect_project(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpVect* v = luaG_pushvect(l);
	*v = cpvproject(*v1, *v2);
	return 1;
}

static int lua_vect_rotate(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpVect* v = luaG_pushvect(l);
	*v = cpvrotate(*v1, *v2);
	return 1;
}

static int lua_vect_unrotate(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpVect* v = luaG_pushvect(l);
	*v = cpvunrotate(*v1, *v2);
	return 1;
}

static int lua_vect_clamp2box(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	cpBB* b = luaG_checkbox(l, 2);

	cpVect* v2 = luaG_pushvect(l);
	*v2 = cpBBClampVect(*b, *v);
	return 1;
}

static int lua_vect_wrap2box(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	cpBB* b = luaG_checkbox(l, 2);

	cpVect* v2 = luaG_pushvect(l);
	*v2 = cpBBWrapVect(*b, *v);
	return 1;
}

static int lua_vect_tostring(lua_State* l)
{
	cpVect* v = luaG_checkvect(l, 1);
	lua_pushfstring(l, "(%f,%f)", v->x, v->y);
	return 1;
}


static const luaL_reg methods[] = {
	{"zero", lua_vect_zero},
	{"new", lua_vect_new},

	{"x", lua_vect_x},
	{"y", lua_vect_y},
	{"setx", lua_vect_setx},
	{"sety", lua_vect_sety},

	{"negate", lua_vect_neg},

	{"dot", lua_vect_dot},

	{"cross", lua_vect_cross},

	{"perp", lua_vect_perp},
	{"rperp", lua_vect_rperp},

	{"project", lua_vect_project},

	{"rotate", lua_vect_rotate},
	{"unrotate", lua_vect_unrotate},

	{"length", lua_vect_length},
	{"lengthsqr", lua_vect_lengthsq},

	{"lerp", lua_vect_lerp},
	{"lerpconst", lua_vect_lerpconst},
	{"slerp", lua_vect_slerp},
	{"slerpconst", lua_vect_slerpconst},

	{"normalize", lua_vect_normalize},

	{"clamp", lua_vect_clamp},

	{"distance", lua_vect_dist},
	{"distancesq", lua_vect_distsq},

	{"near", lua_vect_near},

	{"forangle", lua_vect_forangle},
	{"toangle", lua_vect_toangle},

	{"bclamp", lua_vect_clamp2box},
	{"bwrap", lua_vect_wrap2box},

	{"tostring", lua_vect_tostring},
	{NULL, NULL}
};

static int lua_vect_add(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpVect* sum = luaG_pushvect(l);
	*sum = cpvadd(*v1, *v2);
	return 1;
}

static int lua_vect_sub(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	cpVect* v2 = luaG_checkvect(l, 2);

	cpVect* sum = luaG_pushvect(l);
	*sum = cpvsub(*v1, *v2);
	return 1;
}

static int lua_vect_mul(lua_State* l)
{
	cpVect* v1 = luaG_checkvect(l, 1);
	if (lua_type(l, 2) != LUA_TUSERDATA) {
		cpFloat f = luaL_checknumber(l, 2);
		cpVect* sum = luaG_pushvect(l);
		*sum = cpvmult(*v1, f);
		return 1;
	} else {
		cpVect* v2 = luaG_checkvect(l, 1);
		cpVect* sum = luaG_pushvect(l);
		sum->x = v1->x * v2->x;
		sum->y = v1->y * v2->y;
		return 1;

	}
}

static const luaL_reg meta_methods[] = {
	{"__add", lua_vect_add},
	{"__sub", lua_vect_sub},
	{"__mul", lua_vect_mul},
	{"__len", lua_vect_length},
	{"__eq", lua_vect_eq},
	{"__unm", lua_vect_neg},
	{"__tostring", lua_vect_tostring},
	{NULL, NULL}
};
int register_vector(lua_State *L)
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

