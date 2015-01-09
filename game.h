#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <inttypes.h>

#include "scene.h"

typedef struct refc_ptr refc_ptr;

struct refc_ptr {
	size_t c;
	void* p;

	refc_ptr* next;
	refc_ptr* prev;
};

typedef struct {
	size_t num_scenes;
	scene* scenes;

	refc_ptr* render_data;
	refc_ptr* windows;
} game;

void game_add_scene(game*, scene*);

#endif
