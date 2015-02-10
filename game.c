#include "game.h"

#include <stdlib.h>

#include "global.h"

void game_add_scene(game* g, scene* s)
{
	scene* sn = realloc(g->scenes, g->num_scenes + 1);
	if (sn == NULL) {
		error("Cannot allocate scene");
		return;
	}
	g->scenes = sn;
	memcpy(&(g->scenes[g->num_scenes]), s, sizeof(scene));
	g->num_scenes++;
}

void game_set_current(game* g, scene* s)
{
	g->current = s;
}
