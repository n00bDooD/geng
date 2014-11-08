#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <stdbool.h>

typedef enum {STRING, INT, DOUBLE, BOOL} conf_value_type;

typedef struct {
	const char* name;
	conf_value_type type;

	char* strval; 	// type 0
	int intval;	// type 1
	double dblval;	// type 2
	bool boolval;	// type 3
} property;

void read_configuration(int fd, property* properties);

#endif /* CONFIGURATION_H */

