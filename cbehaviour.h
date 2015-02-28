#ifndef CBEHAVIOUR_H
#define CBEHAVIOUR_H

#include "object.h"

typedef void (*obj_update_func)(object* o, double time_step, void* data);
typedef void (*obj_delete_func)(object* o, void* data);
typedef void* (*obj_create_func)(object* o, lua_State* l);
typedef void (*obj_coll_func)(cpArbiter* arb, void *data);

typedef struct {
	void* data;
	obj_update_func update;
	obj_delete_func delete;
	obj_create_func create;

	// Collision callbacks
	obj_coll_func coll_begin;
	obj_coll_func coll_presolve;
	obj_coll_func coll_postsolve;
	obj_coll_func coll_separate;
} cbehaviour;

void call_update(cbehaviour*, object*, double);
void call_create(cbehaviour*, object*, lua_State*);
void call_delete(cbehaviour*, object*);

cbehaviour* create_behaviour(cbehaviour* template);

#endif /* CBEHAVIOUR_H */

