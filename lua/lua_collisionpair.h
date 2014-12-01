#ifndef LUA_COLLISIONPAIR_H
#define LUA_COLLISIONPAIR_H
#include <lua.h>

typedef struct {
	void* data;
} collision_pair;


collision_pair* luaG_checkcollpair(lua_State*, int index);
collision_pair* luaG_pushcollpair(lua_State*, void*);

int register_collpair(lua_State*);

#endif /* LUA_COLLISIONPAIR_H */
