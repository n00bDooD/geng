#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <chipmunk/chipmunk.h>

#define OBJ_ACTIVE 1
#define OBJ_DELETED 2
#define OBJ_FLIPHOR 2
#define OBJ_FLIPVERT 2

typedef struct {
	char* name;

	/* Flag values
	 * bit 1: active-flag
	 * bit 2: deleted-flag
	 * bit 3: flip-horiz-flag
	 * bit 4: flip-vert-flag
	 */ 
	uint8_t flags;

	cpBody* physics;

	size_t sprite;

	void* tag;

	void* parent;
} object;

#endif /* OBJECT_H */
