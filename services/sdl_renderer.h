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
	SDL_Texture* textures;

	size_t num_sprites;
	sprite* sprites;

	SDL_Renderer* rend;
} sdl_renderer;

void draw_objects(scene*);

#endif /* SDLRENDERER_H */

