#include "cbehaviour.h"

void call_update(cbehaviour* b, object* o, double time_step)
{
	if(b->update != NULL) {
		b->update(o, time_step, b->data);
	}
}

void call_create(cbehaviour* b, object* o, lua_State* l)
{
	if(b->create != NULL) {
		b->data = b->create(o, l);
	}
}

void call_receive(cbehaviour* b, object* o, lua_State* l)
{
	if(b->receive != NULL) {
		b->receive(o, l, b->data);
	}
}

cbehaviour* create_behaviour(cbehaviour* template) {
	cbehaviour* ret = calloc(sizeof(cbehaviour), 1);
	if (ret == NULL) return NULL;

	ret->data = template->data;
	ret->update = template->update;
	ret->delete = template->delete;
	ret->create = template->create;
	ret->receive = template->receive;

	ret->coll_begin = template->coll_begin;
	ret->coll_presolve = template->coll_presolve;
	ret->coll_postsolve = template->coll_postsolve;
	ret->coll_separate = template->coll_separate;

	return ret;
}

void call_delete(cbehaviour* b, object* o)
{
	if(b->delete != NULL) {
		b->delete(o, b->data);
	}
}
