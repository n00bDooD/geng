#include "lua_input.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>

#define TYPE_NAME "input"

inputaxis_data* luaG_checkinput(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	inputaxis_data* s = NULL;
	s = (inputaxis_data*)luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

static int lua_get_input_for_axis(lua_State* l)
{
	int arg = 1;
	inputaxis_data* d = luaG_checkinput(l, arg++);
	const char* name = luaL_checkstring(l, arg++);

	double ret = get_input_for_axis(d, name);
	lua_pushnumber(l, ret);
	return 1;
}

static int lua_create_axis(lua_State* l)
{
	int arg = 1;
	inputaxis_data* d = luaG_checkinput(l, arg++);
	size_t namelen = 0;
	const char* luaname = luaL_checklstring(l, arg++, &namelen);
	if(namelen == 0) {
		luaL_error(l,
		"Axis creation failed: Name cannot be empty.");
	}

	luaL_checktype(l, arg, LUA_TBOOLEAN);
	bool inverted = lua_toboolean(l, arg++);

	double neg_deadz = luaL_checknumber(l, arg++);
	double pos_deadz = luaL_checknumber(l, arg++);

	double neg_max = luaL_checknumber(l, arg++);
	double pos_max = luaL_checknumber(l, arg++);

	char* name = (char*)malloc(namelen + 1);
	if(name == NULL) {
		luaL_error(l,
		   "Axis creation failed: Cannot allocate memory for name");
	}

	memcpy(name, luaname, namelen);
	name[namelen+1] = '\0';

	axis_config* c = (axis_config*)malloc(sizeof(axis_config));
	if(c == NULL) {
		free(name);
		luaL_error(l,
		   "Axis creation failed: Cannot allocate memory for configuration");
	}
	c->invert = inverted;
	c->enabled = true;

	c->negative_deadzone = neg_deadz;
	c->positive_deadzone = pos_deadz;

	c->negative_maximum = neg_max;
	c->positive_maximum = pos_max;

	int res = create_axis(d, name, c);
	if(res == -1) {
		free(name);
		luaL_error(l, "Axis creation failed: Name conflict.");
	}
	return 0;
}

static const luaL_reg methods[] = {
	{"get", lua_get_input_for_axis},
	{NULL, NULL}
};

static const luaL_reg meta_methods[] = {
	{NULL, NULL}
};

int register_input(lua_State *L)
{
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	luaL_openlib(L, 0, meta_methods, 0); /* fill metatable */
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

int register_config_input(lua_State *L)
{
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	luaL_openlib(L, 0, meta_methods, 0); /* fill metatable */
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
