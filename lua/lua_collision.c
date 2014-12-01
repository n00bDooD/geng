#include <stdio.h>
#include <stdbool.h>
#include <lua.h>
#include "../global.h"

#include "lua_colliders.h"
#include "lua_collisionpair.h"
#include "lua_object.h"
#include "../scene.h"

#include <chipmunk/chipmunk.h>

void try_call_func(lua_State* l, cpArbiter* arb, const char* fname)
{
	lua_getglobal(l, fname);
	if(lua_isnil(l, -1)) {
		lua_pop(l, 1);
	} else {
		luaG_pushcollpair(l, arb);
		int res = lua_pcall(l, 1, 0, 0);
		if (res != 0) {
			// TODO: Do sensible stuff here
			fprintf(stderr, "Collision handler error \n");
		}
	}
}

#define CALL_EACH(b, a, f) \
do { \
	if(b != NULL) {\
		size_t idx = 0; \
		while(b[idx].name != NULL) { \
			try_call_func(b[idx++].thread, a, f);\
		} \
	} \
} while(0);

#define GET_BEHAVLISTS(arb, b1, b2) \
behaviour* b1; \
behaviour* b2; \
do{ \
	CP_ARBITER_GET_BODIES(arb, bb1, bb2); \
	b1 = (behaviour*)((object*)cpBodyGetUserData(bb1))->tag; \
	b2 = (behaviour*)((object*)cpBodyGetUserData(bb2))->tag; \
} while(0);

int collision_begin(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_begin");
	CALL_EACH(be2, arb, "collision_begin");
	return true;
}

int collision_preSolve(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_preSolve");
	CALL_EACH(be2, arb, "collision_preSolve");
	return true;
}

void collision_postSolve(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_postSolve");
	CALL_EACH(be2, arb, "collision_postSolve");
}

void collision_separate(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_separate");
	CALL_EACH(be2, arb, "collision_separate");
}

void setup_collision(scene* s)
{
	cpSpaceSetDefaultCollisionHandler(s->physics_data,
			collision_begin,
			collision_preSolve,
			collision_postSolve,
			collision_separate,
			NULL);
}
