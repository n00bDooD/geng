#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <chipmunk/chipmunk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <errno.h>
#include <string.h>

#include "global.h"
#include "object.h"
#include "services.h"
#include "services/sdl_renderer.h"
#include "services/inputaxis.h"
#include "services/sdl_input_shim.h"

#include "lua/lua_audio.h"
#include "lua/lua_input.h"
#include "lua/lua_object.h"
#include "lua/lua_scene.h"
#include "lua/lua_vector.h"
#include "lua/lua_renderer.h"
#include "lua/lua_colliders.h"
#include "lua/lua_collision.h"
#include "lua/lua_collisionpair.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define TICKS_PER_SECOND 125
#define STATIC_TIMESTEP (1.0 / (double)TICKS_PER_SECOND)
#define SKIP_TICKS (STATIC_TIMESTEP * 1000)
#define MAX_FRAMESKIP 5

void luaHandleResult(lua_State* state, int res, char* sorcestr)
{
	if (res != 0) {
		const char* errmesg = lua_tolstring(state, -1, NULL);
		switch (res) {
		case LUA_ERRERR:
			fprintf(stderr,"%s: Lua error handler error: %s\n",
					sorcestr, errmesg);
			break;
		case LUA_ERRMEM:
			fprintf(stderr,"%s: Lua memory error: %s\n",
					sorcestr, errmesg);
			break;
		case LUA_ERRRUN:
			fprintf(stderr,"%s: Runtime lua error: %s\n",
					sorcestr, errmesg);
			break;
		case LUA_ERRSYNTAX:
			fprintf(stderr,"%s: Lua syntax error: %s\n",
					sorcestr, errmesg);
			break;
		default:
			fprintf(stderr,"%s: Lua error: %s\n",
					sorcestr, errmesg);
			break;
		}
	}
}

#if 0
void update_ball(object* o)
{
	input* in = services_get_input();
	double x = in->get_input(in->input_data, "horizontal");
	double y = in->get_input(in->input_data, "vertical");
	cpBodyApplyImpulse(o->transform.rigidbody, cpv(x, y), cpvzero);
}
#endif

char** get_files_in_dir(const char* dir, const char* file_ending, size_t* count)
{
	size_t num = 0;
	char** names = NULL;
	{
		DIR* prefdir = NULL;
		if((prefdir = opendir(dir)) != NULL) {
			struct dirent* d = NULL;
			while((d = readdir(prefdir)) != NULL) {
				char* sub = strchr(d->d_name, '.');
				if(sub != NULL) {
					if(strcmp(sub, file_ending) == 0) {
						char** n = (char**)realloc(names, ++num * sizeof(char*));
						if(n == NULL) error("Read prefab dir");
						names = n;

						*sub = '\0';
						char* new = strdup(d->d_name);
						if(new == NULL) error("strdup");
						*sub = '.';
						names[num-1] = new;
					}
				}
			}
			closedir(prefdir);
		} else {
			error("Open prefab dir");
		}
	}
	if(count != NULL) {
		*count = num;
	}
	return names;
}

int main(int argc, char** argv)
{
	UNUSED(argc);
	UNUSED(argv);

	size_t pool_size = 10000;
	scene* s = (scene*)calloc(1, sizeof(scene));
	s->pool = (object*)calloc(pool_size, sizeof(object));
	if(s->pool == NULL) error("Cannot allocate object pool");
	s->num_objects = pool_size;

	sdl_renderer* sdlrend = (sdl_renderer*)calloc(1, sizeof(sdl_renderer));

	/* ## Set up rendering ## */
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		sdl_error("SDL_Init");
		return -1;
	}

	{
		int flags = MIX_INIT_OGG;
		if ((Mix_Init(flags) & flags) != flags) {
    			fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
			return -1;
		}
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
	                      || SDL_RENDERER_PRESENTVSYNC);
	if(r == NULL) {
		sdl_error("SDL_CreateRenderer");
		return -1;
	}
	sdlrend->rend = r;
	sdlrend->cam.scale = 1;
	sdlrend->cam.x = 0;
	sdlrend->cam.y = 0;
	s->render_data = sdlrend;

	/* ## Set up physics ## */
	cpEnableSegmentToSegmentCollisions();
	cpSpace* spas = cpSpaceNew();
	cpSpaceSetGravity(spas, cpv(0, -98.1 * 4));
	s->physics_data = spas;
	setup_collision(s);

	/* ## Set up input ## */
	inputaxis_data* inpdat = (inputaxis_data*)calloc(1, sizeof(inputaxis_data));
	if(inpdat == NULL) error("Create input data structure");
	inpdat->num_inputaxes = 0;
	inpdat->axes = NULL;
	services_register_input(create_inputaxis(inpdat));

	{
		lua_State* l = luaL_newstate();
		luaL_openlibs(l);
		register_config_input(l, inpdat);
		int res = luaL_dofile(l, "data/input_config.lua");
		luaHandleResult(l, res, "data/input_config.lua");
		lua_close(l);
	}

	{
		lua_State* l2 = luaL_newstate();
		luaL_openlibs(l2);
		register_renderer(l2, sdlrend);
		int res = luaL_dofile(l2, "data/renderer_config.lua");
		luaHandleResult(l2, res, "data/renderer_config.lua");
		lua_close(l2);
	}

	{
		lua_State* l = luaL_newstate();
		luaL_openlibs(l);
		register_config_audio(l);
		int res = luaL_dofile(l, "data/audio_config.lua");
		luaHandleResult(l, res, "data/audio_config.lua");
		lua_close(l);
	}
	{
		lua_State* l3 = luaL_newstate();
		luaL_openlibs(l3);
		register_scene(l3, s);
		register_object(l3);
		register_vector(l3);
		register_colliders(l3);
		register_collpair(l3);
		register_input(l3, inpdat);
		int res = luaL_dofile(l3, "data/scene_init.lua");
		luaHandleResult(l3, res, "data/scene_init.lua");

	}
	//create_axis(inpdat, "horizontal", default_settings());
	//create_axis(inpdat, "vertical", default_settings());

#if 0
	/* ## Set up ground ## */
	cpVect boxverts[4] = {
		cpv(-110,  35),
		cpv( 110,  35),
		cpv( 110, -35),
		cpv(-110, -35)
		};
	o[0].name = "Ground";
	o[0].update = NULL;
	o[0].transform.rigidbody = cpBodyNewStatic();
	cpBodySetPos(o[0].transform.rigidbody, cpv(50, -250));
	cpShape* g = cpPolyShapeNew(o[0].transform.rigidbody, 4, boxverts, cpvzero);
	cpShapeSetFriction(g, 100);
	cpSpaceAddShape(spas, g);

	/*
	int gtexfd = open("kenney/tga/Wood elements/elementWood012.tga", O_RDONLY);
	o[0].sprite = load_tga(r, gtexfd);
	o[0].sprite_offset_x = 0;
	o[0].sprite_offset_y = 0;
	close(gtexfd);
	*/


	
	/* ## Set up hero-ball ## */
	cpFloat radius = 70;
	cpFloat mass = 0.1;

	o[1].name = "Hero ball";
	o[1].update = &update_ball;
	//o[1].sprite_offset_x = radius * 0.5;
//	o[1].sprite_offset_y = radius * -0.5;
	o[1].transform.rigidbody = cpSpaceAddBody(spas, cpBodyNew(mass, cpMomentForCircle(mass, 0, radius, cpvzero)));
	
	cpBodySetPos(o[1].transform.rigidbody, cpv(0, 0));
	cpShape* balls = cpSpaceAddShape(spas, cpCircleShapeNew(o[1].transform.rigidbody, radius, cpvzero));
	cpShapeSetFriction(balls, 0.7);
	
	/*
	int texfd = open("kenney/tga/Aliens/alienBlue_round.tga", O_RDONLY);
	o[1].sprite = load_tga(r, texfd);
	close(texfd);
	*/
#endif

	/* )# Set up control mappings ## */
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

	cpSpaceReindexStatic(spas);

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


			//simulation* sim = services_get_simulation();
			//sim->simulate_step(sim->simulation_data, STATIC_TIMESTEP);
			cpSpaceStep(spas, STATIC_TIMESTEP);

			step_scene(s, STATIC_TIMESTEP);

			// Delete any objects marked DELETED
			cleanup_deleted(s);

			next_game_tick += SKIP_TICKS;
			loops++;
		}
		/*interpolation = (float)(SDL_GetTicks() + SKIP_TICKS - next_game_tick)
				/ (float)SKIP_TICKS;*/
		
		SDL_RenderClear(r);

		// Draw shit
		//draw_objects(num_objects, o);
		draw_objects(s);
		SDL_RenderPresent(r);
	}

	//cpShapeFree(g);
	//cpShapeFree(balls);
	//cpSpaceRemoveBody(spas, o[0].transform.rigidbody);
	//cpSpaceRemoveBody(spas, o[1].transform.rigidbody);
	//cpBodyFree(o[0].transform.rigidbody);
	//cpBodyFree(o[1].transform.rigidbody);

	//SDL_DestroyTexture(o[0].sprite);
	//SDL_DestroyTexture(o[1].sprite);
	//free(o);

	//cpSpaceFree(spas);
	//physics_sim_delete(services_register_simulation(NULL));

	inpdat = delete_inputaxis(services_register_input(NULL));
	if(inpdat->axes != NULL) {
		free(inpdat->axes);
		inpdat->axes = NULL;
	}
	free(inpdat);
	free(control_map);

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	Mix_Quit();
	SDL_Quit();
	return 0;
}
