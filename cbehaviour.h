#ifndef CBEHAVIOUR_H
#define CBEHAVIOUR_H

#include "object.h"

typedef void (*obj_update_func)(object* o, double time_step, void* data);
typedef void (*obj_delete_func)(object* o, void* data);
typedef void* (*obj_create_func)(object* o);

typedef struct {
	void* data;
	obj_update_func update;
	obj_delete_func delete;
	obj_create_func create;
} cbehaviour;

void call_update(cbehaviour*, object*, double);
void call_create(cbehaviour*, object*);
void call_delete(cbehaviour*, object*);

cbehaviour* create_behaviour(cbehaviour* template);

#endif /* CBEHAVIOUR_H */

