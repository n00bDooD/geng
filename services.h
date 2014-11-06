#ifndef SERVICES_H
#define SERVICES_H
#include "object.h"

typedef struct {
	void* render_data;
	void (*draw_object)(void*, object*);
} renderer;

renderer* services_get_renderer();
renderer* services_register_renderer(renderer*);

typedef struct {
	void* simulation_data;
	void (*simulate_step)(void*, double);
} simulation;

simulation* services_get_simulation();
simulation* services_register_simulation(simulation*);


#endif /* SERVICE_H */
