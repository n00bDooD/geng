#ifndef SCENE_H
#define SCENE_H

#include <lua.h>
#include <SDL2/SDL.h>
#include "object.h"

typedef size_t texhandle;
typedef size_t spritehandle;

typedef struct {
	size_t num_objects;
	object* pool;

	lua_State* engine;
} scene;

void* get_scene_property(scene*, const char*);
void set_scene_property(scene*, const char*, void* p);

#define get_scene_physics(x) get_scene_property(x, "physics")
#define get_scene_renderer(x) get_scene_property(x, "renderer")

#define set_scene_physics(x, p) set_scene_property(x, "physics", p)
#define set_scene_renderer(x, p) set_scene_property(x, "renderer", p)

object* get_first_unused(scene*);

object* create_object(scene*);
void delete_object(scene*, object*);

void cleanup_deleted(scene*);

double get_object_posx(object*);
double get_object_posy(object*);
cpVect get_object_pos(object*);
double get_object_angle(object*);
cpVect get_object_velocity(object*);
double get_object_velocity_limit(object*);
double get_object_angular_velocity_limit(object*);
double get_object_angular_velocity(object*);
double get_object_torque(object*);
double get_object_mass(object*);
double get_object_moment(object*);

void set_object_pos(object*, cpVect);
void set_object_angle(object*, double);

void enable_object_physics(object*, double mass, double moment);
void disable_object_physics(object*);

void object_apply_impulse(object*,cpVect,cpVect);
void object_apply_force(object*,cpVect,cpVect);

void object_reset_forces(object*);
cpVect object_convert_local2worldpos(object*, cpVect);
cpVect object_convert_world2localpos(object*, cpVect);

#endif /* SCENE_H */
