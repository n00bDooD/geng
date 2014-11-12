#include "scene.h"


object* get_first_unused(scene* s)
{
	size_t i = 0;
	while(i < s->num_objects) {
		if (!s->pool[i].flags & OBJ_ACTIVE) {
			return &(s->pool[i]);
		}
		i++;
	}
	return NULL;
}

