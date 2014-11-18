#include "scene.h"
#include "global.h"

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
	cpBodySetUserData(o->physics, o);
	o->flags |= OBJ_ACTIVE;
	return o;
}

 /* Delete */

void cpShape_deleter(cpBody* b, cpShape* s, void* d)
{
	UNUSED(b);
	cpSpaceRemoveShape(d, s);
	cpShapeFree(s);
}

void free_physics(object* o)
{
	scene* s = o->parent;
	cpBodyEachShape(o->physics,
			&cpShape_deleter, s->physics_data);

	cpSpaceRemoveBody(s->physics_data, o->physics);
	cpBodyFree(o->physics);
}

void delete_object(scene* s, object* o)
{
	UNUSED(s);
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


double get_object_mass(object* o)
{
	return cpBodyGetMass(o->physics);
}

double get_object_torque(object* o)
{
	return cpBodyGetTorque(o->physics);
}

double get_object_moment(object* o)
{
	return cpBodyGetMoment(o->physics);
}

double get_object_angular_velocity(object* o)
{
	return cpBodyGetAngVel(o->physics);
}

double get_object_angular_velocity_limit(object* o)
{
	return cpBodyGetAngVelLimit(o->physics);
}

double get_object_velocity_limit(object* o)
{
	return cpBodyGetVelLimit(o->physics);
}


void object_apply_force(object* o, cpVect f, cpVect offset)
{
	cpBodyApplyForce(o->physics, f, offset);
}

void object_apply_impulse(object* o, cpVect f, cpVect offset)
{
	cpBodyApplyImpulse(o->physics, f, offset);
}

void object_reset_forces(object* o)
{
	cpBodyResetForces(o->physics);
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

cpVect object_convert_world2localpos(object* o, cpVect v)
{
	return cpBodyWorld2Local(o->physics, v);
}

cpVect object_convert_local2worldpos(object* o, cpVect v)
{
	return cpBodyLocal2World(o->physics, v);
}

