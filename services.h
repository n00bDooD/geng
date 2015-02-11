#ifndef SERVICES_H
#define SERVICES_H
#include "object.h"

typedef struct {
	void* input_data;
	double (*get_input)(void*, const char*);
} input;

input* services_get_input(void);
input* services_register_input(input*);

#endif /* SERVICE_H */
