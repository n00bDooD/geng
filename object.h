#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <chipmunk/chipmunk.h>
#include <SDL2/SDL.h>

typedef struct object object;

typedef struct {
	cpVect position;
	cpFloat angle;
} transform;

union transform_ptr {
	cpBody* rigidbody;
	transform* transform;
};

struct object {
	char* name;

	/* 1, transform points to a rigidbody
	 * 2, transform points to a transform-stucture
	 */
	int transform_type;
	union transform_ptr transform;

	SDL_Texture* sprite;

	void (*update)(object*);
};

cpVect get_object_position(object* o);
cpFloat get_object_angle(object* o);

void draw_objects(size_t objc, object* obj);

#endif /* OBJECT_H */
