#ifndef PHYS_SIM_H
#define PHYS_SIM_H

#include "services.h"
#include <chipmunk/chipmunk.h>

typedef struct {
	cpSpace* space;
} sim_data;

simulation* physics_sim_create(cpSpace* s);
void physics_sim_delete(simulation*);

#endif /* PHYS_SIM_H */
