#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <chipmunk/chipmunk.h>
#include <lua.h>

#define OBJ_ACTIVE 1
#define OBJ_DELETED 2
#define OBJ_FLIPHOR 4
#define OBJ_FLIPVERT 8

typedef struct {
	char* name;
	void* tag;
	void* parent;
	cpBody* physics;

	size_t sprite;

	/* Flag values
	 * bit 1: active-flag
	 * bit 2: deleted-flag
	 * bit 3: flip-horiz-flag
	 * bit 4: flip-vert-flag
	 */ 
	uint8_t flags;
} object;

#endif /* OBJECT_H */
