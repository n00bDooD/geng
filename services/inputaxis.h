#ifndef INPUT_AXIS_H
#define INPUT_AXIS_H

#include "../services.h"
#include <stdbool.h>

typedef struct {
	bool invert;
	bool enabled;

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

axis_config* default_settings();

double get_input_for_axis(void*, const char*);
int update_axis_value(inputaxis_data*, const char* name, double val);
void reset_axis_values(inputaxis_data* d);

int create_axis(inputaxis_data*, const char* name, axis_config*);
int delete_axis(inputaxis_data*, const char* name, axis_config**);

axis_config* get_axis_settings(inputaxis_data*, const char* name);
int set_axis_settings(inputaxis_data*, const char* name, axis_config* settings);

input* create_inputaxis();
inputaxis_data* delete_inputaxis(input*);

#endif /* INPUT_AXIS_H */
