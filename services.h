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

typedef struct {
	void* input_data;
	double (*get_input)(void*, const char*);
} input;

input* services_get_input();
input* services_register_input(input*);

typedef struct {
	void* logic_data;
	void (*update)(void*, object*);
} logic;

logic* services_get_logic();
logic* services_register_logic(logic*);

#endif /* SERVICE_H */
