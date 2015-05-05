#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <chipmunk/chipmunk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <errno.h>
#include <string.h>

#include "global.h"
#include "object.h"
#include "services.h"
#include "messages.h"
#include "services/sdl_renderer.h"
#include "services/inputaxis.h"
#include "services/sdl_input_shim.h"

#include "game.h"

#include "lua/globlua.h"
#include "lua/lua_audio.h"
#include "lua/lua_input.h"
#include "lua/lua_object.h"
#include "lua/lua_scene.h"
#include "lua/lua_vector.h"
#include "lua/lua_renderer.h"
#include "lua/lua_colliders.h"
#include "lua/lua_collision.h"
#include "lua/lua_collisionpair.h"
#include "lua/lua_physics.h"
#include "lua/lua_messaging.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


#define TICKS_PER_SECOND 125
#define STATIC_TIMESTEP (1.0 / (double)TICKS_PER_SECOND)
#define SKIP_TICKS (STATIC_TIMESTEP * 1000)
#define MAX_FRAMESKIP 5

refc_ptr* refcounted_ptr_create(void);
void* refcounted_ptr_pop(refc_ptr*);
game* create_game(void);
inputaxis_data* input_config(void);
int main_scene_step(game* g);


refc_ptr* refcounted_ptr_create()
{
	refc_ptr* p = calloc(1, sizeof(refc_ptr));
	if (p == NULL) error("Create a reference-counted pointer");
	p->next = p;
	p->prev = p;
	return p;
}

void* refcounted_ptr_pop(refc_ptr* p)
{
	void* ret = p->p;
	p->next->prev = p->prev;
	p->prev->next = p->next;
	if (p->prev == p && p->next == p) {
		p->p = NULL;
	} else {
		free(p);
	}
	return ret;
}

game* create_game()
{
	game* g = calloc(1, sizeof(game));
	if (g == NULL) {
		error("Create game structure");
		return NULL;
	}
	g->windows = refcounted_ptr_create();
	g->render_data = refcounted_ptr_create();

	return g;
}

inputaxis_data* input_config()
{
	inputaxis_data* inpdat = (inputaxis_data*)calloc(1, sizeof(inputaxis_data));
	if(inpdat == NULL) error("Create input data structure");
	inpdat->num_inputaxes = 0;
	inpdat->axes = NULL;

	lua_State* inputlua = luaL_newstate();
	if (inputlua == NULL) {
		printerr("Create lua state for input_config");
		free(inpdat);
		return NULL;
	}
	luaL_openlibs(inputlua);
	register_config_input(inputlua, inpdat);
	int res = luaL_dofile(inputlua, "data/input_config.lua");
	plua_error(inputlua, res, "data/input_config.lua");
	lua_close(inputlua);

	return inpdat;
}

int main_scene_step(game* g)
{
	scene* s = g->current;
	void* phys = get_scene_physics(s);
	if (phys != NULL) cpSpaceStep(phys, STATIC_TIMESTEP);
	step_scene(s, STATIC_TIMESTEP);
	// Delete any objects marked DELETED
	cleanup_deleted(s);
	return 0;
}

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

	/* ## Set up rendering ## */
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
		sdl_error("SDL_Init");
		return -1;
	}

	int flags = MIX_INIT_OGG;
	if ((Mix_Init(flags) & flags) != flags) {
    		fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
		return -1;
	}

	SDL_Window* w = SDL_CreateWindow(
	                    "Test",
	                    0,
	                    0,
	                    1200,
	                    800,
	                    SDL_WINDOW_SHOWN);
	if(w == NULL) {
		sdl_error("SDL_CreateWindow");
		return -1;
	}
	SDL_Renderer* r = SDL_CreateRenderer(
	                      w,
	                      -1,
	                      SDL_RENDERER_ACCELERATED
	                      | SDL_RENDERER_PRESENTVSYNC);
	if(r == NULL) {
		sdl_error("SDL_CreateRenderer");
		return -1;
	}

	inputaxis_data* inpdat = input_config();
	if (inpdat == NULL) return -1;
	services_register_input(create_inputaxis(inpdat));

	sdl_renderer* sdlrend = (sdl_renderer*)calloc(1, sizeof(sdl_renderer));
	if (sdlrend == NULL) { error("Create sdl_renderer"); return -1; }
	sdlrend->rend = r;
	sdlrend->cam.scale = 1;
	sdlrend->cam.x = 0;
	sdlrend->cam.y = 0;
	{
		lua_State* l2 = luaL_newstate();
		if (l2 == NULL) {
			printerr("Create lua state for renderer_config");
			return -1;
		}
		luaL_openlibs(l2);
		register_renderer(l2, sdlrend);
		int res = luaL_dofile(l2, "data/renderer_config.lua");
		plua_error(l2, res, "data/renderer_config.lua");
		lua_close(l2);
	}

	sdl_audio* sdlaud = (sdl_audio*)calloc(1, sizeof(sdl_audio));
	sdlaud->chunks = NULL;
	sdlaud->musics = NULL;

	{
		lua_State* l = luaL_newstate();
		if (l == NULL) {
			printerr("Create lua state for audio_config");
			return -1;
		}
		luaL_openlibs(l);
		register_config_audio(l, sdlaud);
		int res = luaL_dofile(l, "data/audio_config.lua");
		plua_error(l, res, "data/audio_config.lua");
		lua_close(l);
	}

	/* ## Set up physics ## */
	cpEnableSegmentToSegmentCollisions();
	cpSpace* spas = cpSpaceNew();


	/* ## Set up messaging ## */
	msgq_state* msgstate = msgq_create(NULL, 1024, 1024, 512, 512);


	game* g = create_game();
	if (g == NULL) return -1;
	g->windows->p = w;
	g->render_data->p = r;
	{
		lua_State* l3 = luaG_newstate(NULL);
		lua_pushlightuserdata(l3, g);
		luaG_setreg(l3, "game");

		lua_pushlightuserdata(l3, spas);
		luaG_setreg(l3, "physics");

		lua_pushlightuserdata(l3, sdlrend);
		luaG_setreg(l3, "renderer");

		luaL_openlibs(l3);
		register_scene(l3, NULL);
		register_object(l3);
		register_vector(l3);
		register_colliders(l3);
		register_collpair(l3);
		register_input(l3, inpdat);
		register_audio(l3, sdlaud);
		register_physics(l3);
		register_renderer(l3, sdlrend);
		register_messaging(l3, msgstate);
		int res = luaL_dofile(l3, "data/init.lua");
		plua_error(l3, res, "data/init.lua");
		lua_close(l3);
	}

	/* ## Set up control mappings ## */
	mapping* control_map = (mapping*)calloc(6, sizeof(mapping));
	if(control_map == NULL) error("Allocate control map");
	control_map[0].key = SDLK_w;
	control_map[0].negative = false;
	control_map[0].axis = "vertical";

	control_map[1].key = SDLK_s;
	control_map[1].negative = true;
	control_map[1].axis = "vertical";

	control_map[2].key = SDLK_d;
	control_map[2].negative = false;
	control_map[2].axis = "horizontal";

	control_map[3].key = SDLK_a;
	control_map[3].negative = true;
	control_map[3].axis = "horizontal";

	control_map[4].key = SDLK_SPACE;
	control_map[4].negative = false;
	control_map[4].axis = "shoot";

	control_map[5].key = 0;
	control_map[5].negative = false;
	control_map[5].axis = NULL;

	int loops = 0;
	uint32_t next_game_tick = SDL_GetTicks();
	SDL_Event e;
	bool quit_requested = false;
	//float interpolation = 0;
	while(!quit_requested) {
		loops = 0;
		while(SDL_GetTicks() > next_game_tick && loops < MAX_FRAMESKIP) {
			while(SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					quit_requested = true;
					break;

				case SDL_WINDOWEVENT:
					break;
				}
			}
			reset_axis_values(inpdat);
			apply_keyboard_input(inpdat, control_map);

			msgq_flush_all(msgstate);

			main_scene_step(g);

			next_game_tick += SKIP_TICKS;
			loops++;
		}
		/*interpolation = (float)(SDL_GetTicks() + SKIP_TICKS - next_game_tick)
				/ (float)SKIP_TICKS;*/
		
		SDL_RenderClear(r);

		// Draw shit
		//draw_objects(num_objects, o);
		draw_objects(g->current);
		SDL_RenderPresent(r);
	}

	for(size_t i = 0; i < g->num_scenes; ++i) {
		for(size_t j = 0; j < g->scenes[i].num_objects; ++j) {
			if((g->scenes[i].pool[j].flags & OBJ_ACTIVE) != 0) {
				delete_object(&(g->scenes[i]), &(g->scenes[i].pool[j]));
			}
		}

		lua_close(g->scenes[i].engine);
		free(g->scenes[i].pool);
	}
	cpSpaceFree(spas);

	free(sdlrend->textures);
	free(sdlrend->sprites);
	free(sdlrend);

	if (sdlaud->chunks != NULL) {
		Mix_Chunk* c = NULL;
		size_t i = 0;
		while((c = sdlaud->chunks[i++]) != NULL) {
			Mix_FreeChunk(c);
		}
		free(sdlaud->chunks);
	}
	if (sdlaud->musics != NULL) {
		Mix_Music* c = NULL;
		size_t i = 0;
		while((c = sdlaud->musics[i++]) != NULL) {
			Mix_FreeMusic(c);
		}
		free(sdlaud->musics);
	}
	free(sdlaud);

	/* These are actually lists... fix it */
	free(g->windows);
	free(g->render_data);

	free(g->scenes);
	free(g);

	free(control_map);
	inpdat = delete_inputaxis(services_register_input(NULL));
	if(inpdat->axes != NULL) {
		for(size_t i = 0; i < inpdat->num_inputaxes; ++i) {
			free((char*)inpdat->axes[i].name);
			free(inpdat->axes[i].settings);
		}
		free(inpdat->axes);
		inpdat->axes = NULL;
	}
	free(inpdat);

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	int count = Mix_QuerySpec(NULL, NULL, NULL);
	while(count-- > 0) {
		Mix_CloseAudio();
	}
	Mix_Quit();
	SDL_Quit();
	return 0;
}
