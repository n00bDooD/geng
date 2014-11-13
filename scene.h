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

	size_t num_prefabs;
	char** prefab_names;
	char** prefabs;
} scene;

object* get_first_unused(scene*);

object* create_object(scene*);
void delete_object(scene*, object*);

double get_object_posx(object*);
double get_object_posy(object*);
cpVect get_object_pos(object*);
double get_object_angle(object*);

void set_object_pos(object*, cpVect);
void set_object_angle(object*, double);

void enable_object_physics(object*, double mass, double moment);
void disable_object_physics(object*);

#endif /* SCENE_H */
