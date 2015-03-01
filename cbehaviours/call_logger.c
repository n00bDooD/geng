#include "call_logger.h"
#include <stdio.h>
#include "../global.h"
#include <lauxlib.h>
#include "../lua/globlua.h"

void update_call_logger(object* o, double time_step, void* data);
void delete_call_logger(object* o, void* data);
void* create_call_logger(object* o, lua_State* l);
void receive_call_logger(object* o, lua_State* l, void* data);

void coll_begin_call_logger(cpArbiter* arb, void* data);
void coll_presolve_call_logger(cpArbiter* arb, void* data);
void coll_postsolve_call_logger(cpArbiter* arb, void* data);
void coll_separate_call_logger(cpArbiter* arb, void* data);

void update_call_logger(object* o, double time_step, void* data)
{
	UNUSED(o);
	UNUSED(time_step);
	UNUSED(data);
	//printf("update_call_logger.\n");
}

void delete_call_logger(object* o, void* data)
{
	UNUSED(o);
	UNUSED(data);
	//printf("delete_call_logger.\n");
}

void* create_call_logger(object* o, lua_State* l)
{
	UNUSED(o);
	int val = luaL_checkint(l, 1);
	UNUSED(val);
	//printf("Supplied value %i\n", val);
	//printf("create_call_logger.\n");
	return NULL;
}

void receive_call_logger(object* o, lua_State* l, void* data)
{
	UNUSED(o);
	UNUSED(data);
	dbg_printstack(l);
}

void coll_begin_call_logger(cpArbiter* arb, void* data)
{
	UNUSED(arb);
	UNUSED(data);
	//printf("coll_begin.\n");
}

void coll_presolve_call_logger(cpArbiter* arb, void* data)
{
	UNUSED(arb);
	UNUSED(data);
	//printf("coll_presolve.\n");
}

void coll_postsolve_call_logger(cpArbiter* arb, void* data)
{
	UNUSED(arb);
	UNUSED(data);
	//printf("coll_postsolve.\n");
}

void coll_separate_call_logger(cpArbiter* arb, void* data)
{
	UNUSED(arb);
	UNUSED(data);
	//printf("coll_separate.\n");
}


static int lua_load_call_logger(lua_State* l)
{
	cbehaviour* template = calloc(sizeof(cbehaviour), 1);
	template->data = NULL;
	template->update = &update_call_logger;
	template->delete = &delete_call_logger;
	template->create = &create_call_logger;

	template->receive = &receive_call_logger;

	template->coll_begin = &coll_begin_call_logger;
	template->coll_presolve = &coll_presolve_call_logger;
	template->coll_postsolve = &coll_postsolve_call_logger;
	template->coll_separate = &coll_separate_call_logger;

	lua_pushlightuserdata(l, template);
	return 1;
}

static const luaL_Reg methods[] = {
	{"load", lua_load_call_logger},
	{NULL, NULL}
};


void register_call_logger(lua_State* l) 
{
	luaL_register(l, "call_logger", methods);
}
