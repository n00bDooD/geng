#ifndef GLOBAL_H
#define GLOBAL_H
#include <errno.h>
#include <stdio.h>

#define error(x) do { fprintf(stderr, "%s %s:%i: %s\n", x, __FILE__, __LINE__, strerror(errno)); } while(0)

#define sdl_error(x) do { fprintf(stderr, "%s %s:%i: %s\n", x, __FILE__, __LINE__, SDL_GetError()); } while(0)

#endif /* GLOBAL_H */
