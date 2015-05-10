#ifndef INPUT_AXIS_H
#define INPUT_AXIS_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	/* Deadzone */
	double negative_deadzone;
	double positive_deadzone;

	/* Max/min */
	double negative_maximum;
	double positive_maximum;

	bool invert;
	bool enabled;
} axis_config;

typedef struct {
	const char* name;
	double value;

	axis_config* settings;
} inputaxis;

typedef struct {
	inputaxis* axes;
	size_t num_inputaxes;
} inputaxis_data;

axis_config* default_settings(void);

double get_input_for_axis(void*, const char*);
int update_axis_value(inputaxis_data*, const char* name, double val);
void reset_axis_values(inputaxis_data* d);

int create_axis(inputaxis_data*, const char* name, axis_config*);
int delete_axis(inputaxis_data*, const char* name, axis_config**);

axis_config* get_axis_settings(inputaxis_data*, const char* name);
int set_axis_settings(inputaxis_data*, const char* name, axis_config* settings);

#endif /* INPUT_AXIS_H */
