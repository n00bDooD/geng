#include "sdl_input_shim.h"

mapping* find_mapping(mapping* maps, SDL_Keycode k);


mapping* find_mapping(mapping* maps, SDL_Keycode k)
{
	size_t idx = 0;
	while(maps[idx].axis != NULL 
		&& maps[idx].key != 0){
		if(maps[idx].key == k) 
			return &(maps[idx]);
		idx++;
	}
	return NULL;
}

void apply_keyboard_input(inputaxis_data* d, mapping* m)
{
	int len = 0;
	const Uint8* s = SDL_GetKeyboardState(&len);

	for(int i = 0; i < len; ++i){
		SDL_Keycode k = SDL_GetKeyFromScancode((SDL_Scancode)i);

		mapping* ma = find_mapping(m, k);
		if (ma != NULL){
			double val = 0;
			if(s[i]) {
				val = 1;
			}
			if (ma->negative) val = -val;
			update_axis_value(d, ma->axis, val);
		}
	}
}

