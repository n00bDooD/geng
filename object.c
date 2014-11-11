#include "object.h"
#include "services.h"
#include "scene.h"
#include "global.h"

object* create_object(scene* s)
{
	object* o = get_first_unused(s);
	o->flags &= OBJ_ACTIVE;
	return o;
}

 /* Delete */

void cpShape_deleter(cpBody* b, cpShape* s, void* d)
{
	cpShapeFree(s);
}

void delete_object(scene* s, object* o)
{
	if(o->flags & OBJ_PHYSICS) {
		cpBodyEachShape(o->transform.rigidbody,
				&cpShape_deleter, NULL);

		cpSpaceRemoveBody(s->physics_data, o->transform.rigidbody);
		cpBodyFree(o->transform.rigidbody);
	} else {
		free(o->transform.transform);
	}
	bzero(o, sizeof(object));
}


 /* Object position */

cpVect get_object_position(object* o)
{
	if(o->flags & OBJ_PHYSICS) {
		return cpBodyGetPos(o->transform.rigidbody);
	} else {
		return o->transform.transform->position;
	}
}

cpFloat get_object_posx(object* o)
{
	return get_object_position(o).x;
}
cpFloat get_object_posy(object* o)
{
	return get_object_position(o).y;
}

void set_object_position(object* o, cpVect* n)
{
	if(o->flags & OBJ_PHYSICS) {
		cpBodySetPos(o->transform.rigidbody, *n);
	} else {
		o->transform.transform->position = *n;
	}
}


 /* Object angle */

cpFloat get_object_angle(object* o)
{
	if(o->flags & OBJ_PHYSICS) {
		cpFloat a = cpBodyGetAngle(o->transform.rigidbody);
		return a;
	} else {
		return o->transform.transform->angle;
	}
}

void set_object_angle(object* o, cpFloat a)
{
	if(o->flags & OBJ_PHYSICS) {
		cpBodySetAngle(o->transform.rigidbody, a);
	} else {
		o->transform.transform->angle = a;
	}
}

