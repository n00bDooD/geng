#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <chipmunk/chipmunk.h>

#define OBJ_ACTIVE 1
#define OBJ_PHYSICS 2

typedef struct {
	char* name;

	/* Flag values
	 * bit 1: active-flag
	 */ 
	uint8_t flags;

	cpBody* physics;

	size_t sprite;

	void* tag;

	void* parent;
} object;

#endif /* OBJECT_H */
