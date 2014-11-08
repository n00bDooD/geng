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

#include "global.h"
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

void update_ball(object* o)
{
	input* in = services_get_input();
	double x = in->get_input(in->input_data, "horizontal");
	double y = in->get_input(in->input_data, "vertical");
	cpBodyApplyForce(o->transform.rigidbody, cpv(x, y), cpvzero);
}

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
	size_t num_objects = 2;
	object* o = (object*)calloc(num_objects, sizeof(object));
	if(o == NULL) {
		error("Allocate objects");
	}

	/* ## Set up rendering ## */
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		sdl_error("SDL_Init");
		return -1;
	}

	SDL_Window* w = SDL_CreateWindow(
	                    "Test",
	                    0,
	                    0,
	                    400,
	                    400,
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

	services_register_renderer(sdl_renderer_create(w, r));


	/* ## Set up input ## */
	inputaxis_data* inpdat = (inputaxis_data*)calloc(1, sizeof(inpdat));
	if(inpdat == NULL) error("Create input data structure");
	inpdat->num_inputaxes = 0;
	inpdat->axes = NULL;
	services_register_input(create_inputaxis(inpdat));

	create_axis(inpdat, "horizontal", default_settings());
	create_axis(inpdat, "vertical", default_settings());

	
	/* ## Set up physics ## */
	cpSpace* spas = cpSpaceNew();
	cpSpaceSetGravity(spas, cpv(0, -100));
	services_register_simulation(physics_sim_create(spas));

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

	int gtexfd = open("kenney/tga/Wood elements/elementWood012.tga", O_RDONLY);
	o[0].sprite = load_tga(r, gtexfd);
	o[0].sprite_offset_x = 0;
	o[0].sprite_offset_y = 0;
	close(gtexfd);


	
	/* ## Set up hero-ball ## */
	cpFloat radius = 70;
	cpFloat mass = 0.1;

	o[1].name = "Hero ball";
	o[1].update = &update_ball;
	o[1].sprite_offset_x = radius * 0.5;
	o[1].sprite_offset_y = radius * -0.5;
	o[1].transform.rigidbody = cpSpaceAddBody(spas, cpBodyNew(mass, cpMomentForCircle(mass, 0, radius, cpvzero)));
	
	cpBodySetPos(o[1].transform.rigidbody, cpv(0, 0));
	cpShape* balls = cpSpaceAddShape(spas, cpCircleShapeNew(o[1].transform.rigidbody, radius, cpvzero));
	cpShapeSetFriction(balls, 0.7);
	
	int texfd = open("kenney/tga/Aliens/alienBlue_round.tga", O_RDONLY);
	o[1].sprite = load_tga(r, texfd);
	close(texfd);


	/* )# Set up control mappings ## */
	mapping* control_map = (mapping*)calloc(5, sizeof(mapping));
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

	control_map[4].key = 0;
	control_map[4].negative = false;
	control_map[4].axis = NULL;

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

			update_objects(num_objects, o);

			simulation* sim = services_get_simulation();
			sim->simulate_step(sim->simulation_data, STATIC_TIMESTEP);

			next_game_tick += SKIP_TICKS;
			loops++;
		}
		/*interpolation = (float)(SDL_GetTicks() + SKIP_TICKS - next_game_tick)
				/ (float)SKIP_TICKS;*/
		
		SDL_RenderClear(r);

		// Draw shit
		draw_objects(num_objects, o);
		SDL_RenderPresent(r);
	}

	cpShapeFree(g);
	cpShapeFree(balls);
	cpBodyFree(o[0].transform.rigidbody);
	cpBodyFree(o[1].transform.rigidbody);

	SDL_DestroyTexture(o[0].sprite);
	SDL_DestroyTexture(o[1].sprite);
	free(o);

	cpSpaceFree(spas);
	physics_sim_delete(services_register_simulation(NULL));

	inpdat = delete_inputaxis(services_register_input(NULL));
	if(inpdat->axes != NULL) {
		free(inpdat->axes);
		inpdat->axes = NULL;
	}
	free(inpdat);
	free(control_map);

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	sdl_renderer_delete(services_register_renderer(NULL));
	SDL_Quit();
	return 0;
}
