#ifndef LUA_COLLIDERS_H
#define LUA_COLLIDERS_H

#include <lua.h>
#include <chipmunk/chipmunk.h>

#include "../object.h"

typedef enum { NONE, UNKNOWN, CIRCLE, LINE, POLY } collider_type;

typedef struct {
	cpShape* shape;
	collider_type type;
} collider;

int register_colliders(lua_State*);

collider* luaG_checkcoll(lua_State* L, int index);
collider* luaG_pushcoll(lua_State* L);

collider* object_add_circle(object*, double, cpVect);
collider* object_add_segment(object*, cpVect, cpVect, double);
collider* object_add_box(object*, double, double);
collider* object_add_poly(object*, cpVect* verts, int num_verts, cpVect);

cpVect polycoll_get_centroid(cpVect* verts, int num_verts);
int polycoll_validate(cpVect* verts, int num_verts);
void polycoll_center(cpVect* verts, int num_verts);

#endif /* LUA_COLLIDERS_H */
