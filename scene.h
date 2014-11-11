#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL.h>
#include "object.h"

typedef size_t texhandle;
typedef size_t spritehandle;

typedef struct {
	size_t num_objects;
	object* pool;

	void* render_data;

	void* physics_data;
} scene;

object* get_first_unused(scene*);

object* create_object(scene*);
void delete_object(scene*, object*);

#endif /* SCENE_H */
