#include "lua_copy.h"
#include <lualib.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <string.h>

#define SEEN_LIST_KEY "foobarbaz"

#define UNUSED(x) (void)(x)

int dumpwriter(lua_State* l, const void* p, size_t sz, void* ud);
const char* dumpreader(lua_State* l, void* data, size_t* sz);
int copy_lua_value(lua_State* a, lua_State* b);

/*
 * Copies a lua table, adding it to the
 * seen state of both states.
 */
void copy_lua_table(lua_State*, lua_State*);
/* 
 * Resets the tracking of already-copied
 * values.
 */
void reset_seen_state(lua_State*);
/*
 * Pushes the already seen value at the
 * specified index to the top of the
 * stack
 */
void get_seen_table(lua_State*, size_t);
/*
 * Returns the index of an already seen
 * value.
 */
size_t is_table_seen(lua_State*);
/*
 * Adds the value at the top of the stack
 * to the list of seen objects.
 */
void see_table(lua_State*);

/*
 * Copies a lua functions upvalues from l1 to l2.
 * Requires that the lua function has been copied over
 * and is at the top of both l1 and l2.
 * copy_lua_function calls this.
 */
int transfer_upvalues(lua_State* l1, lua_State* l2);
/*
 * Copies a lua function, i.e. a block of lua
 * code from l1 to l2
 */
int copy_lua_function(lua_State*, lua_State*);
/*
 * Copies a lua userdata by value.
 * The copy is a shallow copy, so all pointers will
 * still have the original objects values.
 */
void copy_userdata(lua_State*, lua_State*);



void get_seen_table(lua_State* l, size_t idx)
{
	lua_getfield(l, LUA_REGISTRYINDEX, SEEN_LIST_KEY);
	if(lua_isnil(l, -1) || lua_objlen(l, -1) < idx) {
		lua_pop(l, 1);
		lua_pushnil(l);
		return;
	}
	lua_rawgeti(l, -1, (int)idx);
	lua_remove(l, -2);
}

void reset_seen_state(lua_State* l)
{
	// In lua, setting the value to nil is
	// effectively clearing it.
	lua_pushnil(l);
	lua_setfield(l, LUA_REGISTRYINDEX, SEEN_LIST_KEY);
}

size_t is_table_seen(lua_State* l)
{
	lua_getfield(l, LUA_REGISTRYINDEX, SEEN_LIST_KEY);
	if(lua_isnil(l, -1)) {
		// No copy-list equals no
		// seen values.
		lua_pop(l, 1);
		return 0;
	}
	size_t table_size = lua_objlen(l, -1);
	// Lua tables begin at one
	for(size_t i = table_size; i > 0; --i) {
		lua_rawgeti(l, -1, (int)i);
		int is_equal = lua_rawequal(l, -3, -1);
		lua_pop(l, 1);
		if(is_equal) {
			lua_pop(l, 1);
			return i;
		}
	}
	// Not in list
	lua_pop(l, 1);
	return 0;
}

void see_table(lua_State* l)
{
	// Make a copy of the value to save
	lua_pushvalue(l, -1);

	lua_getfield(l, LUA_REGISTRYINDEX, SEEN_LIST_KEY);
	if(lua_isnil(l, -1)) {
		// Create seen-list if there
		// is'nt one created.
		lua_pop(l, 1);
		lua_createtable(l, 1, 0);
	}
	size_t table_size = lua_objlen(l, -1);
	lua_insert(l, -2);
	// Lua tables begin at 1
	lua_rawseti(l, -2, (int)++table_size);
	lua_setfield(l, LUA_REGISTRYINDEX, SEEN_LIST_KEY);
}

struct cdata {
	size_t len;
	char* data;
};
int dumpwriter(lua_State* l, const void* p, size_t sz, void* ud)
{
        UNUSED(l);
        struct cdata* d = (struct cdata*)ud;
        void* t = realloc(d->data, d->len + sz);
        d->data = t;
        memcpy(d->data + d->len, p, sz);
        d->len += sz;
        return 0;
}
const char* dumpreader(lua_State* l, void* data, size_t* sz)
{
        UNUSED(l);
        struct cdata* d = (struct cdata*)data;
        *sz = d->len;
        return d->data;
}

int transfer_upvalues(lua_State* l1, lua_State* l2)
{
	int i = 1;
	while(lua_getupvalue(l1, -1, i++) != NULL) {
		int res = copy_lua_value(l1, l2);
		lua_pop(l1, 1);
		if (res == 0) {
			if (lua_setupvalue(l2, -2, i-1) == NULL) {
				// Upvalue setting failed. Pop it
				lua_pop(l2, 1);
			}
		}
	}
	return 0;
}

int copy_lua_function(lua_State* l1, lua_State* l2)
{
	struct cdata dat = { 0, NULL };
	int dumpres = lua_dump(l1, dumpwriter, &dat);
	if(dumpres != 0) {
        	return dumpres;
	}
	int loadres = lua_load(l2, dumpreader, &dat, NULL);
	if(loadres != 0) {
		return -loadres;
	}
	// Transfer the function upvalues too
	int transres = transfer_upvalues(l1, l2);
        free(dat.data);
	if(transres != 0) {
		return transres;
	}
	return 0;
}


// TODO: Copy metatables for userdata
void copy_userdata(lua_State* a, lua_State* b)
{
	size_t len = lua_objlen(a, -1);
	void* new = lua_newuserdata(b, len);
	void* tmp = lua_touserdata(a, -1);
	memcpy(new, tmp, len);
}

// TODO: Use seen-tracking for reference values.
//	TODO: Figure out what values are by-reference.
int copy_lua_value(lua_State* a, lua_State* b)
{
	switch(lua_type(a, -1)) {
		case LUA_TNIL:
			// Nil does not need to be 'copied'
			lua_pushnil(b);
			break;
		case LUA_TNUMBER: // Pass-by-value
			lua_pushnumber(b, lua_tonumber(a, -1));
			break;
		case LUA_TBOOLEAN: // Pass-by-value
			lua_pushboolean(b, lua_toboolean(a, -1));
			break;
		case LUA_TSTRING: // Pass-by-effectively-value
			lua_pushstring(b, lua_tolstring(a, -1, NULL));
			break;
		case LUA_TTABLE: // Pass-by-reference
			{size_t seeidx = is_table_seen(a);
			if(seeidx == 0) {
				see_table(a);
				copy_lua_table(a, b);
				see_table(b);
			} else {
				get_seen_table(b, seeidx);
			}
			break;}
		case LUA_TFUNCTION: // Pass-by-reference
			{size_t seeidx = is_table_seen(a);
			if(seeidx == 0) {
				see_table(a);
				int res = copy_lua_function(a, b);
				if(res != 0)
					lua_pushnil(b);
				see_table(b);
				if (res != 0) return res;
			} else {
				get_seen_table(b, seeidx);
			}
			break;}
		case LUA_TTHREAD: // Pass-by-reference
			// TODO: Can this be fixed?
			//lua_pushthread(b, lua_tothread(a, -1));
			return 1;
		case LUA_TUSERDATA: // Pass-by-reference
			copy_userdata(a, b);
			break;
		case LUA_TLIGHTUSERDATA: // Pass-by-value
			lua_pushlightuserdata(b, lua_touserdata(a, -1));
			break;
		case LUA_TNONE:
		default:
			// Shouldnt get here
			// Log maybe?
			break;
	}
	return 0;
}

void copy_lua_table(lua_State* l1, lua_State* l2)
{
	// Create a new table to copy to.
        lua_newtable(l2);
	see_table(l2);

	// Begin iterating though keys
        lua_pushnil(l1);
        while(lua_next(l1, -2) != 0) {
		// Swap key and value locations
		lua_insert(l1, -2);

		// copy key
		if(copy_lua_value(l1, l2) == 0) {
			// swap key and values back,
			// as the key needs to remain on the stack
			lua_insert(l1, -2);
			if(copy_lua_value(l1, l2) == 0) {
				// Copy OK, save value
                		lua_rawset(l2, -3);
			} else {
				// Copy failed
				// Pop new key from stack
				lua_pop(l2, 1);
			}
		} else {
			// Key copy failed
			// Swap key & value
			lua_insert(l1, -2);
		}
		
		// Pop value and leave key on stack,
		// to be used in next iter
                lua_pop(l1, 1);
        }
	// Leave original and new table on stack.
	// Let the caller decide what to do with them
}


int luaExt_copy(lua_State* a, lua_State* b)
{
	int res = copy_lua_value(a, b);
	reset_seen_state(a);
	reset_seen_state(b);
	return res;
}
