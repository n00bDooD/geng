#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <chipmunk/chipmunk.h>

typedef struct {
	cpVect position;
	cpFloat angle;
} transform;

union transform_ptr {
	cpBody* rigidbody;
	transform* transform;
};

#define OBJ_ACTIVE 1
#define OBJ_PHYSICS 2

typedef struct {
	char* name;

	/* Flag values
	 * bit 1: active-flag
	 * bit 2: transform_ptr type
	 */ 
	uint8_t flags;
	union transform_ptr transform;

	size_t sprite;

	void* parent;
} object;

cpFloat get_object_posx(object* o);
cpFloat get_object_posy(object* o);
cpVect get_object_position(object* o);
cpFloat get_object_angle(object* o);

#endif /* OBJECT_H */
