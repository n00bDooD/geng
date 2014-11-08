#include "services.h"
#include "global.h"

void do_nothing_drawer(void* null, object* dontcare)
{
	// What to do here? Nothing!
	return;
}

renderer* get_null_drawer()
{
	renderer* re = (renderer*)malloc(sizeof(renderer));
	if(re == NULL) error("get_null_drawer");
	re->render_data = NULL;
	re->draw_object = &do_nothing_drawer;
	return re;
}

renderer* cur_rend = NULL;

renderer* services_get_renderer()
{
	if(cur_rend == NULL) {
		services_register_renderer(get_null_drawer());
	}
	return cur_rend;
}


renderer* services_register_renderer(renderer* r)
{
	renderer* old = cur_rend;
	if (old != NULL && old->render_data == NULL && old->draw_object == &do_nothing_drawer) {
		free(old);
		old = NULL;
	}
	cur_rend = r;
	return old;
}

void do_nothing_sim(void* null, double ts)
{
	return;
}

simulation* get_null_simulation()
{
	simulation* sim = (simulation*)malloc(sizeof(simulation));
	if(sim == NULL) error("get_null_simulation");
	sim->simulation_data = NULL;
	sim->simulate_step = &do_nothing_sim;

	return sim;
}

simulation* cur_sim = NULL;

simulation* services_get_simulation()
{
	if(cur_sim == NULL) {
		services_register_simulation(get_null_simulation());
	}
	return cur_sim;
}

simulation* services_register_simulation(simulation* s)
{
	simulation* old = cur_sim;
	if(old != NULL && old->simulation_data == NULL && old->simulate_step == &do_nothing_sim) {
		free(old);
		old = NULL;
	}
	cur_sim = s;
	return old;
}

double do_nothing_input(void* null, const char* secretmessage)
{
	return 0;
}

input* get_null_input()
{
	input* i = (input*)malloc(sizeof(input));
	if(i == NULL) error("get_null_input");
	i->input_data = NULL;
	i->get_input = &do_nothing_input;

	return i;
}

input* cur_input = NULL;

input* services_get_input()
{
	if(cur_input == NULL) {
		services_register_input(get_null_input());
	}
	return cur_input;
}

input* services_register_input(input* i)
{
	input* old = cur_input;
	if(old != NULL && old->input_data == NULL && old->get_input == &do_nothing_input) {
		free(old);
		old = NULL;
	}
	cur_input = i;
	return old;
}
