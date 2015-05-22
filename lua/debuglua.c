#define _POSIX_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

char* jsonenc(const char* orig) {
	size_t len = strlen(orig);

	size_t i = 0;
	for (size_t i = 0; i < len; ++i) {
		switch(orig[i]) {
			case '"':
			case '\\':
			case '\b':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
				goto reencode;
		}
	}

	return NULL;
reencode: {
	char* new = malloc(len * 2);
	memcpy(new, orig, i);

	size_t newl = i;
	size_t j = i;
	for (j = i; j < len; ++j) {
		switch(i) {
			case '"':
				new[newl++] = '\\';
				new[newl++] = '"';
				break;
			case '\\':
				new[newl++] = '\\';
				new[newl++] = '\\';
				break;
			case '\b':
				new[newl++] = '\\';
				new[newl++] = 'b';
				break;
			case '\f':
				new[newl++] = '\\';
				new[newl++] = 'f';
				break;
			case '\n':
				new[newl++] = '\\';
				new[newl++] = 'n';
				break;
			case '\r':
				new[newl++] = '\\';
				new[newl++] = 'r';
				break;
			case '\t':
				new[newl++] = '\\';
				new[newl++] = 't';
				break;
			default:
				new[newl++] = orig[j];
				break;
		}
	}
	return new;
	}
}

#define w(x) write(fdesc, x, strlen(x));
#define ws(x) do { \
	char* n = jsonenc(x); \
	w(n == NULL ? x : n); \
	if (n != NULL) free(n); \
} while(0)
#define wint(x) do { \
			char intbuf[sizeof(int)*8+1] = {'\0'}; \
			sprintf(intbuf, "%i", x); \
			write(fdesc, intbuf, strlen(intbuf)); \
		} while(0)
void write_debug(int fdesc, lua_Debug* d)
{
	if (d == NULL) {
		w("null");
		return;
	}

	w(",\"event\":");
	wint(d->event);

	w("{\"name\":");
	if (d->name != NULL) {
		w("\"");
		ws(d->name);
		w("\"");
	} else w("null");
	w(",\"namewhat\":\"");
	ws(d->namewhat);
	w("\"");

	if (d->what != NULL) {
		w(",\"what\":\"");
		ws(d->what);
		w("\"");
	}

	if (d->source != NULL) {
		w(",\"source\":\"");
		w(d->source);
		w("\"");
	}

	w(",\"currentline\":");
	wint(d->currentline);

	w(",\"nups\":");
	wint(d->nups);

	w(",\"linedefined\":");
	wint(d->linedefined);

	w(",\"lastlinedefined\":");
	wint(d->lastlinedefined);
	w(",\"namewhat\":\"");
	ws(d->namewhat);
	w("\"");

	w("}\n");
}
#undef w

void lua_debughook(lua_State* l, lua_Debug* d)
{
	if (lua_getinfo(l, "nSlu", d) > 0) {
		write_debug(1, d);
	}
}

void setup_debug(lua_State* l)
{
	//lua_sethook(l, &lua_debughook, LUA_MASKCOUNT, 1);
}

