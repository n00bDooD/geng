#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "global.h"

#define VALUE_STRING_MAX 1024
#define PROPERTY_NAME_MAX 128
#define COMMENT_CHAR '#'

typedef struct {
	const char* name;
	int type;
	char* strval;
	int intval;
	double dblval;
	bool boolval;
} property;

property* get_property(property* props, const char* name)
{
	ssize_t idx = -1;
	while(props[++idx].name != NULL){
		if(strcmp(props[idx].name, name) == 0) {
			return &(props[idx]);
		}
	}
	return NULL;
}

void trim_quotes(char* str)
{
	char* buf = (char*)malloc(strlen(str));

	int state = 0;
	size_t idx = 0;
	size_t residx = 0;
	while(str[idx++] != '\0') {
		switch(str[idx]) {
			case '"':
				if(state == 2)
					goto justaddit;
				else if (state == 0)
					state = 1;
				else
					state = 0;
				break;
			case '\'':
				if(state == 1) 
					goto justaddit;
				else if (state == 0)
					state = 2;
				else
					state = 0;
				break;
justaddit:
			default:
				buf[residx++] = str[idx];
		}
	}

	strcpy(str, buf);
	free(buf);
}

int read_config_line(int fd, property* properties)
{
	size_t pidx = 0;
	char* property_name = (char*)malloc(PROPERTY_NAME_MAX);
	bzero(property_name, PROPERTY_NAME_MAX);

	size_t vidx = 0;
	char* value_str = (char*)malloc(VALUE_STRING_MAX);
	bzero(value_str, VALUE_STRING_MAX);

	void* cb = malloc(1);
	if(cb == NULL) error("read_config_line");
	int state = 0;
	ssize_t read_result = 0;
again:
	while((read_result = read(fd, cb, 1)) > 0){
		char c = *(char*)cb;
		if(c == '\n') goto complete;
		switch(state) {
			case 0: /* Read property name */
				if(c == COMMENT_CHAR){
					state = 4; 
					continue;
				}
				if(!isspace((unsigned char)c)) {
					if(c == '=') {
						state = 1;
					} else {
						property_name[pidx++] = c;
					}
				}
				break;
			case 1: /* Read property value */
				if(c == COMMENT_CHAR){
					state = 4; 
					continue;
				}
				if(!isspace((unsigned char)c)) {
					if(c == '"') state = 2;
					if(c == '\'') state = 3;
					value_str[vidx++] = c;
				}
				break;
			case 2: /* Read property value in ""s */
				if(c == '"') state = 1;
				value_str[vidx++] = c;
				break;
			case 3: /* Read property value in ''s */
				if(c == '\'') state = 1;
				value_str[vidx++] = c;
				break;
			case 4:
				/* Continue until end of line */
				break;
		}
	}
	if(read_result == -1) {
		if(errno == EAGAIN)
			goto again;
		else
			error("read_config_line");
	} else if (read_result == 0) {
		free(cb);
		return 1;
	}

complete:
	free(cb);
	cb = NULL;
	if(pidx == 0) {
		/* No property name.
		 * Most likely an empty line
		 * ignore.
		 */
		free(property_name);
		free(value_str);
		return 2;
	} else {
		property* p = get_property(properties, property_name);
		if(p == NULL) {
			/* ERROR!
			 * Unknown property
			 */
			free(property_name);
			free(value_str);
			return 3;
		}
		if(vidx == 0) {
			/* ERROR!
			 * Value required
			 */
			free(property_name);
			free(value_str);
			return 4;
		}

		switch(p->type) {
			case 0: {
				/* char* */
				trim_quotes(value_str);
				if(p->strval != NULL) {
					free(p->strval);
					p->strval = NULL;
				}
				size_t vallen = strlen(value_str);
				p->strval = (char*)malloc(vallen + 1);
				strncpy(p->strval, value_str, vallen);
				p->strval[vallen + 1] = '\0';
				break;
			}
			case 1: {
				/* int */
				p->intval = atoi(value_str);
				break;
			}
			case 2: {
				/* double */
				p->dblval = strtod(value_str, NULL);
				break;
			}
			case 3: {
				/* bool */
				for(size_t i = 0; i < vidx; ++i) { value_str[i] = tolower(value_str[i]); }
				if(strcmp(value_str, "true") == 0){
					p->boolval = true;
				} else if(strcmp(value_str, "false") == 0) {
					p->boolval = false;
				} else if(strcmp(value_str, "yes") == 0){
					p->boolval = true;
				} else if(strcmp(value_str, "no") == 0) {
					p->boolval = false;
				} else if(strcmp(value_str, "t") == 0) {
					p->boolval = true;
				} else if(strcmp(value_str, "f") == 0) {
					p->boolval = false;
				} else if(strcmp(value_str, "y") == 0) {
					p->boolval = true;
				} else if(strcmp(value_str, "n") == 0) {
					p->boolval = false;
				} else if(atoi(value_str) != 0) {
					p->boolval = true;
				} else {
					p->boolval = false;
				}
				break;
			}
		}
		free(property_name);
		free(value_str);
		return 0;
	}
}


void test_configuration_read()
{
	int fd = open("ball_template.gobj", O_RDONLY);
	if(fd == -1) {
		error("test_configuration_read");
		return;
	}
	property* props = (property*)calloc(9, sizeof(property));

	props[0].name = "name";
	props[0].type = 0;

	props[1].name = "sprite.source";
	props[1].type = 0;

	props[2].name = "physics.shapes";
	props[2].type = 0;

	props[3].name = "sourcefile";
	props[3].type = 0;

	props[4].name = "physics.mass";
	props[4].type = 2;

	props[5].name = "physics";
	props[5].type = 3;

	props[6].name = "sprite.offset.x";
	props[6].type = 1;

	props[7].name = "sprite.offset.y";
	props[7].type = 1;

	props[8].name = NULL;
	props[8].type = 0;

	while(true) {
		int result = read_config_line(fd, props);
		if(result == 0) {
			// We read a line
		} else if (result == 2) {
			// Empty line/comment only
		} else if (result == 1) {
			// Done
			break;
		}
	}
}
