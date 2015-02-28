#include <stdio.h>
#include <stdbool.h>
#include <lua.h>
#include "../global.h"

#include "lua_colliders.h"
#include "lua_collisionpair.h"
#include "lua_object.h"
#include "../scene.h"

#include <chipmunk/chipmunk.h>


void try_call_func(lua_State* l, cpArbiter* arb, const char* fname, phys_callback current);
void try_call_cfunc(cbehaviour* b, cpArbiter* arb, phys_callback current);
int collision_begin(cpArbiter* arb, cpSpace* space, void* data);
int collision_preSolve(cpArbiter* arb, cpSpace* space, void* data);
void collision_postSolve(cpArbiter* arb, cpSpace* space, void* data);
void collision_separate(cpArbiter* arb, cpSpace* space, void* data);
void setup_collision(scene* s);

void try_call_func(lua_State* l, cpArbiter* arb, const char* fname, phys_callback current)
{
	lua_getglobal(l, fname);
	if(lua_isnil(l, -1)) {
		lua_pop(l, 1);
	} else {
		collision_pair* cp = luaG_pushcollpair(l, arb);
		cp->current = current;
		int res = lua_pcall(l, 1, 0, 0);
		plua_error(l, res, "collision handler");
	}
}

void try_call_cfunc(cbehaviour* b, cpArbiter* arb, phys_callback current)
{
	switch(current) {
		case COLL_BEGIN:
			if (b->coll_begin != NULL) {
				b->coll_begin(arb, b->data);
			}
			break;
		case COLL_PRESOLVE:
			if (b->coll_presolve != NULL) {
				b->coll_presolve(arb, b->data);
			}
			break;
		case COLL_POSTSOLVE:
			if (b->coll_postsolve != NULL) {
				b->coll_postsolve(arb, b->data);
			}
			break;
		case COLL_SEP:
			if (b->coll_separate != NULL) {
				b->coll_separate(arb, b->data);
			}
			break;
		case COLL_NONE:
			break;
	}
}

#define CALL_EACH(b, a, f, c) \
do { \
	if(b != NULL) {\
		size_t idx = 0; \
		while(b[idx].name != NULL) { \
			if(b[idx].script_behaviour) { \
				try_call_func(b[idx++].content.thread, a, f, c);\
			} else { \
				try_call_cfunc(b[idx++].content.beh, a, c);\
			} \
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

	CALL_EACH(be1, arb, "collision_begin", COLL_BEGIN);
	CALL_EACH(be2, arb, "collision_begin", COLL_BEGIN);
	return true;
}

int collision_preSolve(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_preSolve", COLL_PRESOLVE);
	CALL_EACH(be2, arb, "collision_preSolve", COLL_PRESOLVE);
	return true;
}

void collision_postSolve(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_postSolve", COLL_POSTSOLVE);
	CALL_EACH(be2, arb, "collision_postSolve", COLL_POSTSOLVE);
}

void collision_separate(cpArbiter* arb, cpSpace* space, void* data)
{
	UNUSED(data); UNUSED(space);
	GET_BEHAVLISTS(arb, be1, be2);

	CALL_EACH(be1, arb, "collision_separate", COLL_SEP);
	CALL_EACH(be2, arb, "collision_separate", COLL_SEP);
}

void setup_collision(scene* s)
{
	cpSpaceSetDefaultCollisionHandler(get_scene_physics(s),
			collision_begin,
			collision_preSolve,
			collision_postSolve,
			collision_separate,
			NULL);
}
