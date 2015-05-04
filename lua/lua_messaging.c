#include "lua_messaging.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "lua_copy.h"
#include "globlua.h"
#include "global.h"

#include <string.h>

#define TYPE_NAME "messaging"
#define REGISTRY_KEY "geng.messaging"

void set_messaging_registry(lua_State* L, msgq_state* d);

msgq_state* luaG_checkmessage(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	msgq_state* s = NULL;
	s = luaL_checkudata(L, index, TYPE_NAME);
	if (s == NULL) luaL_typerror(L, index, TYPE_NAME);
	return s;
}

msgq_state* get_message_registry(lua_State* L)
{
	lua_pushstring(L, REGISTRY_KEY);
	lua_rawget(L, LUA_REGISTRYINDEX);
	msgq_state* ret = lua_touserdata(L, -1);
	lua_pop(L, 1);
	if(ret == NULL) {
		luaL_error(L, "Could not find messaging data");
	}
	return ret;
}

static int lua_broadcast_message(lua_State* l)
{
	msgq_state* s = get_message_registry(l);
	const char* message = luaL_checklstring(l, 1, 0);
	if (message == NULL)
		luaL_error(l, "Valid message needed");
	lua_pushvalue(l, 2);
	luaL_ref(l, LUA_REGISTRYINDEX);
	msgq_broadcast(s, l, message, l);
	return 0;
}

struct listen_data {
	lua_State* state;
	int listen_method;
};

void lua_listener_handler(void* me, void* sender, void* data)
{
	UNUSED(data);
	struct listen_data* l = me;
	lua_State* other = sender;

	lua_rawgeti(l->state, LUA_REGISTRYINDEX, l->listen_method);
	luaExt_copy(l->state, other);
	luaG_pcall(l->state, 1, 0);
}

static int lua_listen_message(lua_State* l)
{
	msgq_state* s = get_message_registry(l);
	const char* message = luaL_checklstring(l, 1, 0);
	if (message == NULL)
		luaL_error(l, "Valid message needed");

	struct listen_data* d = malloc(sizeof(struct listen_data));
	d->state = l;
	lua_pushvalue(l, 2);
	luaL_checktype(l, 2, LUA_TFUNCTION);
	d->listen_method = luaL_ref(l, LUA_REGISTRYINDEX);

	msgq_listen(s, d, message, &lua_listener_handler);
	return 0;
}

static const luaL_Reg methods[] = {
	{"broadcast", lua_broadcast_message},
	{"listen", lua_listen_message},
	{NULL, NULL}
};

void set_messaging_registry(lua_State* L, msgq_state* d)
{
	lua_pushstring(L, REGISTRY_KEY);
	lua_pushlightuserdata(L, d);
	/* registry['geng.input'] = d */
	lua_rawset(L, LUA_REGISTRYINDEX);
}

int register_messaging(lua_State *L, msgq_state* d)
{
	set_messaging_registry(L, d);
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

