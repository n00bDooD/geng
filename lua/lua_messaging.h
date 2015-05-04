#ifndef LUAMESSAGING_H
#define LUAMESSAGING_H
#include <lua.h>

#include "../messages.h"

int register_messaging(lua_State*, msgq_state*);

msgq_state* get_message_registry(lua_State*);

msgq_state* luaG_checkmessage(lua_State*, int);

#endif /* LUAMESSAGING_H */
