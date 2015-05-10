#define _DEFAULT_SOURCE
#include "animation.h"
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include "../global.h"
#include <lauxlib.h>
#include "../lua/globlua.h"
#include "../messages.h"

#define FLIPX 1
#define FLIPY 2
#define HASFLIPX 4
#define HASFLIPY 8

#define min(a, b) (a < b ? a : b)

typedef struct {
	size_t sprite;
	char flip;
} anim_frame;

typedef struct {
	char* name;
	double speed;
	anim_frame* frames;
	size_t num_frames;
} animation;

typedef struct {
	animation* animations;
	size_t num_animations;
	size_t current;
	double time;
} animation_state;

void update_animation(object* o, double time_step, void* data);
void delete_animation(object* o, void* data);
void* create_animation(object* o, lua_State* l);

int get_frame(lua_State* l, anim_frame*);
int get_animation(lua_State* l, animation*, const char*);

void animation_change(void* me, void* sender, void* data);


void update_animation(object* o, double time_step, void* data)
{
	UNUSED(o);
	UNUSED(time_step);
	animation_state* a = data;

	if (a == NULL || a->animations == NULL) return;

	animation* an = &(a->animations[a->current]);

	a->time += time_step * an->speed;
	if (a->time >= an->num_frames) {
		a->time -= an->num_frames;
	}
	size_t anim_idx = min((size_t)lrint(a->time), an->num_frames-1);
	anim_frame f = an->frames[anim_idx];
	
	o->sprite = f.sprite;
	if (f.flip & HASFLIPX) {
		if (f.flip & FLIPX) {
			o->flags |= OBJ_FLIPHOR;
		} else {
			o->flags &= ~OBJ_FLIPHOR;
		}
	}

	if (f.flip & HASFLIPY) {
		if (f.flip & FLIPY) {
			o->flags |= OBJ_FLIPVERT;
		} else {
			o->flags &= ~OBJ_FLIPVERT;
		}
	}
}

void delete_animation(object* o, void* data)
{
	UNUSED(o);
	animation_state* a = data;

	for(size_t i = 0; i < a->num_animations; ++i) {
		free(a->animations[i].name);
		free(a->animations[i].frames);
	}
	free(a->animations);
	free(a);
}

int get_frame(lua_State* l, anim_frame* fr)
{
	size_t sprite = 0;
	char flipvals = 0;
	if(lua_type(l, -1) == LUA_TTABLE) {
		lua_pushliteral(l, "sprite");
		lua_rawget(l, -2);
		sprite = (size_t)lua_tointeger(l, -1);
		lua_pop(l, 1);

		lua_pushliteral(l, "fliph");
		lua_rawget(l, -2);
		if(!lua_isnil(l, -1)) {
			if(lua_toboolean(l, -1)) {
				flipvals |= FLIPX;
			} else {
				flipvals &= ~FLIPX;
			}
			flipvals |= HASFLIPX;
		} else {
			flipvals &= ~HASFLIPX;
		}
		lua_pop(l, 1);

		lua_pushliteral(l, "flipv");
		lua_rawget(l, -2);
		if(!lua_isnil(l, -1)) {
			if(lua_toboolean(l, -1)) {
				flipvals |= FLIPY;
			} else {
				flipvals &= ~FLIPY;
			}
			flipvals |= HASFLIPY;
		} else {
			flipvals &= ~HASFLIPY;
		}
		lua_pop(l, 1);
	} else {
		sprite = (size_t)lua_tointeger(l, -1);
	}
	if (sprite > 0) {
		fr->sprite = sprite;
		fr->flip = flipvals;
		return 0;
	} else {
		return -1;
	}
}

int get_animation(lua_State* l, animation* a, const char* name)
{
	lua_pushliteral(l, "speed");
	lua_rawget(l, -2);
	double speed = 1.0;
	if (!lua_isnil(l, -1)) {
		speed = lua_tonumber(l, -1);
	}
	lua_pop(l, 1);

	if (name == NULL) {
		lua_pushliteral(l, "name");
		lua_rawget(l, -2);
		name = lua_tolstring(l, -1, NULL);
		if (name == NULL) {
			return -1;
		}
		lua_pop(l, 1);
	}

	lua_pushliteral(l, "frames");
	lua_rawget(l, -2);
	if(lua_isnil(l, -1)) {
		return -1;
	}
	size_t num_frames = lua_objlen(l, -1);
	anim_frame* frames = calloc(sizeof(anim_frame), num_frames);
	for(size_t f = 1; f <= num_frames; ++f) {
		lua_rawgeti(l, -1, (int)f);
		
		if(get_frame(l, &(frames[f-1])) != 0) {
			free(frames);
			return -1;
		}

		lua_pop(l, 1);
	}

	lua_pop(l, 1);

	a->name = strdup(name);
	a->speed = speed;
	a->frames = frames;
	a->num_frames = num_frames;

	return 0;
}

void* create_animation(object* o, lua_State* l)
{
	UNUSED(o);
	UNUSED(l);
	const char* def_name = luaL_optstring(l, 2, NULL);

	luaL_checktype(l, 1, LUA_TTABLE);

	// Trim extra args
	lua_settop(l, 1);

	size_t current = 0;
	size_t num_animations = lua_objlen(l, 1);
	animation* anims = NULL;
	if (num_animations > 0)  {
		anims = calloc(sizeof(animation), num_animations);

		for(size_t i = 1; i <= num_animations; ++i) {
			lua_rawgeti(l, 1, (int)i);

			if(get_animation(l, &(anims[i-1]), NULL) != 0) {
				free(anims);
				luaL_error(l, "Invalid argument");
			}

			if (def_name != NULL && strcmp(anims[i-1].name,
						       def_name) == 0) {
				current = i-1;
			}

			lua_pop(l, 1);
		}
	} else {
		lua_pushnil(l);
		while(lua_next(l, -2) != 0) {
			anims = realloc(anims, sizeof(animation) * (++num_animations));

			if(lua_type(l, -2) != LUA_TSTRING) {
				free(anims);
				luaL_error(l, "Invalid argument");
			}
			const char* name = lua_tolstring(l, -2, NULL);
			if (name == NULL) {
				free(anims);
				luaL_error(l, "Invalid argument");
			}
		
			if(get_animation(l, &(anims[num_animations-1]), name) != 0) {
				free(anims);
				luaL_error(l, "Invalid argument");
			}

			lua_pop(l, 1);
		}
	}

	animation_state* a = malloc(sizeof(animation_state));
	a->animations = anims;
	a->num_animations = num_animations;
	a->current = current;
	a->time = 0;

	msgq_listen(o->messaging, a, "animation", &animation_change);

	return a;
}

void animation_change(void* me, void* sender, void* data)
{
	int refarg = (int)data;
	lua_State* l = sender;

	lua_rawgeti(l, LUA_REGISTRYINDEX, refarg);
	if(lua_type(l, -1) == LUA_TSTRING) {
		const char* anim = luaL_optstring(l, -1, NULL);
		if (anim == NULL) return;
		lua_pop(l, 1);

		animation_state* a = me;
		for(size_t i = 0; i < a->num_animations; ++i) {
			if(strcmp(a->animations[i].name, anim) == 0) {
				a->current = i;
				break;
			}
		}
	}
}

static int lua_load_animation(lua_State* l)
{
	cbehaviour* template = calloc(sizeof(cbehaviour), 1);
	template->data = NULL;
	template->update = &update_animation;
	template->delete = &delete_animation;
	template->create = &create_animation;


	template->coll_begin = NULL;
	template->coll_presolve = NULL;
	template->coll_postsolve = NULL;
	template->coll_separate = NULL;

	lua_pushlightuserdata(l, template);
	return 1;
}

static const luaL_Reg methods[] = {
	{"load", lua_load_animation},
	{NULL, NULL}
};


void register_animation(lua_State* l) 
{
	luaL_register(l, "animation", methods);
}
