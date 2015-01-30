#ifndef GLOBAL_H
#define GLOBAL_H
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define UNUSED(x) (void)(x)

#define error(x) do { fprintf(stderr, "%s %s:%i: %s\n", x, __FILE__, __LINE__, strerror(errno)); } while(0)

#define sdl_error(x) do { fprintf(stderr, "%s %s:%i: %s\n", x, __FILE__, __LINE__, SDL_GetError()); } while(0)

#define lua_error(l, r, w) do { \
	if (r != 0) { \
		const char* lua_error_error_message = lua_tolstring(l, -1, NULL); \
		switch (r) { \
		case LUA_ERRERR: \
			fprintf(stderr,"%s: Lua error handler error: %s\n", \
					w, lua_error_error_message); \
			break; \
		case LUA_ERRMEM: \
			fprintf(stderr,"%s: Lua memory error: %s\n", \
					w, lua_error_error_message); \
			break; \
		case LUA_ERRRUN: \
			fprintf(stderr,"%s: Runtime lua error: %s\n", \
					w, lua_error_error_message); \
			break; \
		case LUA_ERRSYNTAX: \
			fprintf(stderr,"%s: Lua syntax error: %s\n", \
					w, lua_error_error_message); \
			break; \
		default: \
			fprintf(stderr,"%s: Lua error: %s\n", \
					w, lua_error_error_message); \
			break; \
		} \
	} \
} while(0)

#endif /* GLOBAL_H */
