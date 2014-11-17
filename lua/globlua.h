#ifndef GLOBLUA_H
#define GLOBLUA_H

#include <lua.h>
#include "../scene.h"
#include "../services/inputaxis.h"

void luaG_register_all(lua_State*, scene*, inputaxis_data*);

#endif /* GLOBLUA_H */
