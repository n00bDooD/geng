#ifndef LUAINPUT_H
#define LUAINPUT_H
#include <lua.h>

#include "../services/inputaxis.h"

int register_input(lua_State*, inputaxis_data*);

int register_config_input(lua_State*, inputaxis_data*);

inputaxis_data* luaG_checkinput(lua_State*, int);

#endif /* LUAINPUT_H */
