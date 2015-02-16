#include "game.h"

#include <stdlib.h>

#include "global.h"

void game_add_scene(game* g, scene* s)
{
	scene* sn = realloc(g->scenes, (g->num_scenes + 1) * sizeof(scene));
	if (sn == NULL) {
		error("Cannot allocate scene");
		return;
	}
	g->scenes = sn;

	g->scenes[g->num_scenes].pool = s->pool;
	g->scenes[g->num_scenes].engine = s->engine;
	g->scenes[g->num_scenes].num_objects = s->num_objects;

	g->num_scenes++;
}

void game_set_current(game* g, scene* s)
{
	g->current = s;
}
