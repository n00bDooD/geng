#ifndef SDL_INPUT_SHIM_H
#define SDL_INPUT_SHIM_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "services/inputaxis.h"

typedef struct {
	SDL_Keycode key;
	bool negative;
	const char* axis;
} mapping;

void apply_keyboard_input(inputaxis_data* d, mapping* maps);

#endif /* SDL_INPUT_SHIM_H */
