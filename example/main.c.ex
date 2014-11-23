#include "lua_copy.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define UNUSED(x) (void)(x)

lua_State* b;

int do_copy(lua_State* l)
{
	lua_getglobal(b, "printc");
	if(lua_isnil(b, -1)) {
		luaL_error(l, "Cannot find method to call");
	}
	luaExt_copy(l, b);
	int prntres = lua_pcall(b, 1, 0, 0);
	if(prntres != 0) {
		luaL_error(l, lua_tolstring(b, -1, NULL));
	}
	return 0;
}

static const luaL_reg funcs[] = {
	{"docopy", do_copy},
	{NULL, NULL},
};

int main(int argc, char **argv)
{
	UNUSED(argc); UNUSED(argv);
	lua_State* a = luaL_newstate();
	b = luaL_newstate();
	luaL_openlibs(a);
	luaL_openlibs(b);

	luaL_register(a, "m", funcs);

	int res = luaL_dofile(b, "consumer.lua");
	if(res != 0) {
		const char* err = lua_tolstring(b, -1, NULL);
		fprintf(stderr, "load b: %s\n", err);
		return res;
	}
	res = luaL_dofile(a, "provider.lua");
	if(res != 0) {
		const char* err = lua_tolstring(a, -1, NULL);
		fprintf(stderr, "load a: %s\n", err);
		return res;
	}
	return res;
}
