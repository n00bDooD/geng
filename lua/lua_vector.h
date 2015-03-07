#ifndef LUAVECT_H
#define LUAVECT_H
#include <lua.h>
#include <chipmunk/chipmunk.h>

int register_vector(lua_State*);

cpVect* luaG_checkvect(lua_State* L, int index);
cpVect* luaG_optvect(lua_State* L, int index, cpVect* nil);
cpVect* luaG_pushvect(lua_State* L);

#endif /* LUAVECT_H */

