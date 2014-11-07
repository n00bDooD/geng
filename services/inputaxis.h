#ifndef INPUT_AXIS_H
#define INPUT_AXIS_H

#include "../services.h"
#include <stdbool.h>

typedef struct {
	bool invert;

	/* Deadzone */
	double negative_deadzone;
	double positive_deadzone;

	/* Max/min */
	double negative_maximum;
	double positive_maximum;
} axis_config;

typedef struct {
	const char* name;
	double value;

	axis_config* settings;
} inputaxis;

typedef struct {
	size_t num_inputaxes;
	inputaxis* axes;
} inputaxis_data;

int create_axis(inputaxis_data*, const char* name, axis_config*);
int delete_axis(inputaxis_data*, const char* name, axis_config**);

input* create_inputaxis();
inputaxis_data* delete_inputaxis(input*);

#endif /* INPUT_AXIS_H */
