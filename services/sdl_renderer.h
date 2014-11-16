#ifndef SDLRENDERER_H
#define SDLRENDERER_H

#include "../scene.h"

typedef struct {
	texhandle tex;
	int offset_x;
	int offset_y;
} sprite;

typedef struct {
	size_t num_textures;
	SDL_Texture** textures;

	size_t num_sprites;
	sprite* sprites;

	texhandle background;

	SDL_Renderer* rend;
} sdl_renderer;

void draw_objects(scene*);

texhandle sdl_renderer_add_texture(sdl_renderer*, SDL_Texture*);
spritehandle sdl_renderer_add_sprite(sdl_renderer*, texhandle, int xoff, int yoff);

SDL_Texture* create_tex_from_file(sdl_renderer*, const char* name);

#endif /* SDLRENDERER_H */

