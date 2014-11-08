#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chipmunk/chipmunk.h>
#include <SDL2/SDL.h>
#include <errno.h>

#include "object.h"
#include "services.h"
#include "services/sdl_renderer.h"
#include "services/physics_sim.h"
#include "services/inputaxis.h"
#include "sdl_input_shim.h"

#include "tga.h"

#define TICKS_PER_SECOND 25
#define STATIC_TIMESTEP (1.0 / (double)TICKS_PER_SECOND)
#define SKIP_TICKS (STATIC_TIMESTEP * 1000)
#define MAX_FRAMESKIP 5

SDL_Texture* load_tga(SDL_Renderer* r, int fd)
{
	targa_file* tga = tga_readfile(fd);
	if (tga == NULL) fprintf(stderr, "%s\n", strerror(errno));

	SDL_Texture* tex = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, tga->head.width, tga->head.height);
	//if(tex == NULL) SDL_Error();

	SDL_UpdateTexture(tex, NULL, tga->image_data, tga->head.width * sizeof(char) * 4);
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

	free(tga);
	return tex;
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	mapping* control_map = (mapping*)calloc(5, sizeof(control_map));
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

	control_map[4].key = 0;
	control_map[4].negative = false;
	control_map[4].axis = NULL;



	SDL_Window* w = SDL_CreateWindow(
	                    "Test",
	                    50,
	                    50,
	                    400,
	                    400,
	                    SDL_WINDOW_SHOWN);
	SDL_Renderer* r = SDL_CreateRenderer(
	                      w,
	                      -1,
	                      SDL_RENDERER_ACCELERATED
	                      || SDL_RENDERER_PRESENTVSYNC);

	services_register_renderer(sdl_renderer_create(w, r));


	inputaxis_data* inpdat = (inputaxis_data*)calloc(1, sizeof(inpdat));
	services_register_input(create_inputaxis(inpdat));

	create_axis(inpdat, "horizontal", default_settings());
	create_axis(inpdat, "vertical", default_settings());

	cpSpace* spas = cpSpaceNew();
	cpSpaceSetGravity(spas, cpv(0, -100));
	services_register_simulation(physics_sim_create(spas));

	cpShape* g = cpSegmentShapeNew(spas->staticBody, cpv(-20, -250), cpv(180, -250), 0);
	cpShapeSetFriction(g, 1);
	cpSpaceAddShape(spas, g);

	object* o = (object*)calloc(1, sizeof(object));
	o->transform.rigidbody = cpSpaceAddBody(spas, cpBodyNew(1, cpMomentForCircle(1, 0, 128, cpvzero)));
	cpBodySetPos(o->transform.rigidbody, cpv(0, 0));
	cpShape* balls = cpSpaceAddShape(spas, cpCircleShapeNew(o->transform.rigidbody, 128, cpvzero));
	cpShapeSetFriction(balls, 0.7);
	
	int texfd = open("planet.tga", O_RDONLY);
	o->sprite = load_tga(r, texfd);
	close(texfd);

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

			input* in = services_get_input();
			double x = in->get_input(in->input_data, "horizontal") * 100;
			double y = in->get_input(in->input_data, "vertical") * 100;
			cpBodyApplyForce(o->transform.rigidbody, cpv(x, y), cpvzero);

			simulation* sim = services_get_simulation();
			sim->simulate_step(sim->simulation_data, STATIC_TIMESTEP);

			next_game_tick += SKIP_TICKS;
			loops++;
		}
		//interpolation = (float)(SDL_GetTicks() + SKIP_TICKS - next_game_tick) / (float)SKIP_TICKS;
		
		SDL_RenderClear(r);

		/*
		cpVect pos = cpBodyGetPos(o->transform.rigidbody);
    		cpVect vel = cpBodyGetVel(o->transform.rigidbody);
    		printf(
				"Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f). a %5.2f\n",
				(double)next_game_tick - SKIP_TICKS, pos.x, pos.y, vel.x, vel.y, cpBodyGetAngle(o->transform.rigidbody)
				);
				*/

		// Draw shit
		draw_objects(1, o);
		SDL_RenderPresent(r);
	}

	cpShapeFree(g);
	cpShapeFree(balls);
	cpBodyFree(o->transform.rigidbody);

	SDL_DestroyTexture(o->sprite);
	free(o);

	cpSpaceFree(spas);
	physics_sim_delete(services_register_simulation(NULL));

	inpdat = delete_inputaxis(services_register_input(NULL));
	if(inpdat->axes != NULL) {
		free(inpdat->axes);
		inpdat->axes = NULL;
	}
	free(inpdat);

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	sdl_renderer_delete(services_register_renderer(NULL));
	SDL_Quit();
	return 0;
}
