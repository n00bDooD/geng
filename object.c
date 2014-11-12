#include "object.h"
#include "services.h"
#include "scene.h"
#include "global.h"

object* create_object(scene* s)
{
	object* o = get_first_unused(s);
	o->parent = s;
	o->flags |= OBJ_ACTIVE;
	o->transform.transform = (transform*)calloc(1,
				sizeof(transform));
	return o;
}

 /* Delete */

void cpShape_deleter(cpBody* b, cpShape* s, void* d)
{
	cpShapeFree(s);
}

void free_physics(object* o)
{
	cpBodyEachShape(o->transform.rigidbody,
			&cpShape_deleter, NULL);

	scene* s = o->parent;
	cpSpaceRemoveBody(s->physics_data, o->transform.rigidbody);
	cpBodyFree(o->transform.rigidbody);
}

void delete_object(scene* s, object* o)
{
	if(o->flags & OBJ_PHYSICS) {
		free_physics(o);
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

cpBool object_has_physics(object* o)
{
	return o->flags & OBJ_PHYSICS;
}

void set_object_physics(object* o, cpBool b)
{
	/* Don't set physics if it already has em */
	if((o->flags & OBJ_PHYSICS) == b) return;
	if(o->flags & OBJ_PHYSICS) {
		transform* t = (transform*)malloc(sizeof(transform));
		if(t == NULL) error("set_object_physics");
		t->position = cpBodyGetPos(o->transform.rigidbody);
		t->angle = cpBodyGetAngle(o->transform.rigidbody);
		free_physics(o);

		o->flags |= OBJ_PHYSICS;
	} else {
		cpBody* b = cpBodyNew(1, 1); //TODO: Add physobj-getter
		cpBodySetPos(b, o->transform.transform->position);
		cpBodySetAngle(b, o->transform.transform->angle);
		free(o->transform.transform);

		o->flags &= ~OBJ_PHYSICS;
	}
}

