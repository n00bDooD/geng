#include "cbehaviour.h"

void call_update(cbehaviour* b, object* o, double time_step)
{
	if(b->update != NULL) {
		b->update(o, time_step, b->data);
	}
}

void call_create(cbehaviour* b, object* o)
{
	if(b->update != NULL) {
		b->data = b->create(o);
	}
}

cbehaviour* create_behaviour(cbehaviour* template) {
	cbehaviour* ret = calloc(sizeof(cbehaviour), 1);
	if (ret == NULL) return NULL;

	ret->data = template->data;
	ret->update = template->update;
	ret->delete = template->delete;
	ret->create = template->create;

	return ret;
}

void call_delete(cbehaviour* b, object* o)
{
	if(b->delete != NULL) {
		b->delete(o, b->data);
	}
}
