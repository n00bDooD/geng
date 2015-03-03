#ifndef GLOBLUA_H
#define GLOBLUA_H

#include <lua.h>
#include <lauxlib.h>
#include "../scene.h"
#include "../services/inputaxis.h"
#include "../services/sdl_audio.h"

#define BEHAVIOUR_RECEIVE_MNAME "receive"
#define BEHAVIOUR_UPDATE_MNAME "update"
#define BEHAVIOUR_DELETE_MNAME "delete"
#define BEHAVIOUR_RECEIVE_MNAME "receive"

#define BEHAVIOUR_COLL_BEGIN_MNAME "collision_begin"
#define BEHAVIOUR_COLL_PRESOLVE_MNAME "collision_preSolve"
#define BEHAVIOUR_COLL_POSTSOLVE_MNAME "collision_postSolve"
#define BEHAVIOUR_COLL_SEPARATE_MNAME "collision_separate"

void luaG_register_all(lua_State*, scene*, inputaxis_data*,sdl_audio*);

void luaG_getreg(lua_State*, const char*);
void luaG_setreg(lua_State*, const char*);
void luaG_copy_state(lua_State* from, lua_State* to);
void luaG_init_state(lua_State*);
lua_State* luaG_newstate(lua_State*);

void dbg_printstack(lua_State*);
void dbg_printtable(lua_State*, int);

int luaG_pcall(lua_State* l, int nargs, int nresults);

#endif /* GLOBLUA_H */
