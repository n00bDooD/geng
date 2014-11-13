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

object* create_object(scene* s)
{
	object* o = get_first_unused(s);
	o->parent = s;
	o->physics = cpBodyNewStatic();
	o->flags |= OBJ_ACTIVE;
	return o;
}

 /* Delete */

void cpShape_deleter(cpBody* b, cpShape* s, void* d)
{
	cpShapeFree(s);
}

void free_physics(object* o)
{
	cpBodyEachShape(o->physics,
			&cpShape_deleter, NULL);

	scene* s = o->parent;
	cpSpaceRemoveBody(s->physics_data, o->physics);
	cpBodyFree(o->physics);
}

void delete_object(scene* s, object* o)
{
	free_physics(o);
	bzero(o, sizeof(object));
}



double get_object_posx(object* o)
{
	return cpBodyGetPos(o->physics).x;
}
double get_object_posy(object* o)
{
	return cpBodyGetPos(o->physics).y;
}

cpVect get_object_pos(object* o)
{
	return cpBodyGetPos(o->physics);
}

double get_object_angle(object* o)
{
	return cpBodyGetAngle(o->physics);
}



void set_object_pos(object* o, cpVect p)
{
	cpBodySetPos(o->physics, p);
}

void set_object_angle(object* o, double a)
{
	cpBodySetAngle(o->physics, a);
}

void disable_object_physics(object* o)
{
	if(!cpBodyIsStatic(o->physics)) {
		cpSpace* s = ((scene*)o->parent)->physics_data;
		cpSpaceRemoveBody(s, o->physics);
		cpSpaceConvertBodyToStatic(s, o->physics);
	}
}

void enable_object_physics(object* o, double mass, double moment)
{
	if(cpBodyIsStatic(o->physics)) {
		cpSpace* s = ((scene*)o->parent)->physics_data;
		cpSpaceConvertBodyToDynamic(s, o->physics, mass, moment);
		cpSpaceAddBody(s, o->physics);
	}
}
