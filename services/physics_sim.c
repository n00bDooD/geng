#include "physics_sim.h"
#include "../global.h"

void simulate_sim(void* data, double dt)
{
	sim_data* s = (sim_data*)data;
	cpSpaceStep(s->space, dt);
}

simulation* physics_sim_create(cpSpace* s)
{
	simulation* ns = (simulation*)malloc(sizeof(simulation));
	if(ns == NULL) error("physics_sim_create");
	sim_data* nsd = (sim_data*)malloc(sizeof(sim_data));
	if(nsd == NULL) error("physics_sim_create data");
	nsd->space = s;
	ns->simulation_data = nsd;
	ns->simulate_step = &simulate_sim;
	return ns;
}

void physics_sim_delete(simulation* s)
{
	sim_data* old = (sim_data*)s->simulation_data;
	free(old);
	free(s);
}
