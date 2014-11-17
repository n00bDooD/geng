#include "lua_input.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>

#define TYPE_NAME "input"
#define REGISTRY_KEY "geng.input"

inputaxis_data* luaG_checkinput(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	inputaxis_data* s = NULL;
	s = (inputaxis_data*)luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

inputaxis_data* get_input_registry(lua_State* L)
{
	lua_pushstring(L, REGISTRY_KEY);
	lua_rawget(L, LUA_REGISTRYINDEX);
	inputaxis_data* ret = (inputaxis_data*)lua_touserdata(L, -1);
	if(ret == NULL) {
		luaL_error(L, "Could not find input data");
	}
	return ret;
}

static int lua_get_input_for_axis(lua_State* l)
{
	int arg = 1;
	inputaxis_data* d = get_input_registry(l);
	const char* name = luaL_checkstring(l, arg++);

	double ret = get_input_for_axis(d, name);
	lua_pushnumber(l, ret);
	return 1;
}

static int lua_create_axis(lua_State* l)
{
	inputaxis_data* d = get_input_registry(l);
	size_t namelen = 0;
	const char* luaname = luaL_checklstring(l, 1, &namelen);
	if(namelen == 0) {
		luaL_error(l,
		"Axis creation failed: Name cannot be empty.");
	}

	luaL_checktype(l, 2, LUA_TBOOLEAN);
	bool inverted = lua_toboolean(l, 2);

	double neg_deadz = luaL_checknumber(l, 3);
	double pos_deadz = luaL_checknumber(l, 4);

	double neg_max = luaL_checknumber(l, 5);
	double pos_max = luaL_checknumber(l, 6);

	char* name = (char*)malloc(namelen+1);
	if(name == NULL) {
		luaL_error(l,
		   "Axis creation failed: Cannot allocate memory for name");
	}

	memcpy(name, luaname, namelen);
	name[namelen] = '\0';

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

static const luaL_Reg methods[] = {
	{"get", lua_get_input_for_axis},
	{NULL, NULL}
};

static const luaL_Reg private_methods[] = {
	{"create", lua_create_axis},
	{NULL, NULL}
};

void set_input_registry(lua_State* L, inputaxis_data* d)
{
	lua_pushstring(L, REGISTRY_KEY);
	lua_pushlightuserdata(L, d);
	/* registry['geng.input'] = d */
	lua_rawset(L, LUA_REGISTRYINDEX);
}

int register_input(lua_State *L, inputaxis_data* d)
{
	set_input_registry(L, d);
	/* Create methods table & add it to globals */
	luaL_openlib(L, TYPE_NAME, methods, 0);
	/* Create metatable for object, and add it to registry */
	luaL_newmetatable(L, TYPE_NAME);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3); 		/* duplicate methods table */
	lua_rawset(L, -3);		/* metatable.__index = methods */

	lua_pop(L, 1); 			/* drop metatable */
	return 1;			/* leave methods on stack */
}

int register_config_input(lua_State *L, inputaxis_data* d)
{
	set_input_registry(L, d);

	luaL_register(L, TYPE_NAME, private_methods);
	lua_pop(L, 1);
	return 0;
}
