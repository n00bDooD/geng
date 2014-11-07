#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include <SDL2/SDL.h>
#include "../object.h"
#include "../services.h"

typedef struct {
	SDL_Window* w;
	SDL_Renderer* r;
} sdl_renderer_data;

void sdl_renderer_draw(void* renderer_data, object* obj);

renderer* sdl_renderer_create(SDL_Window* w, SDL_Renderer* r);
void sdl_renderer_delete(renderer*);

#endif /* SDL_RENDERER_H */
