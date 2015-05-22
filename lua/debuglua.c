#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../global.h"

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
		if (d->source[0] == '@' && d->source[1] != '/') {
			char wd[PATH_MAX+1] = {'\0'};
			getcwd(wd, PATH_MAX+1);
			wd[strlen(wd)] = '/';
			strncat(wd, d->source + 1, PATH_MAX+1);

			w("@");
			w(wd);
		} else {
			w(d->source);
		}
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

size_t read_command_tobuf(int fd, char* line, size_t len)
{
	size_t linelen = 0;

	while(linelen < len) {
		char c = 0;
		switch(read(fd, &c, 1)) {
		case 0:
			// EOF or something. Assume eol
			line[linelen] = '\0';
			return linelen;
		case 1:
			if (c == '\n') {
				// Done
				line[linelen] = '\0';
				return linelen;
			} else {
				line[linelen++] = c;
			}
			break;
		default:
			// Error
			return 0;
		}
	}
	return linelen;
}

void read_command(int fd, lua_State* l)
{
	char line[2048] = {'\0'};
	line[0] = 'p';
	line[1] = 'r';
	line[2] = 'i';
	line[3] = 'n';
	line[4] = 't';
	line[5] = '(';
	size_t cmdlen = read_command_tobuf(fd, line + 6, 2048 - 6) + 6;
	do {
		if (cmdlen > 0) {
			line[cmdlen] = ')';
			line[cmdlen+1] = '\0';
			int strload = luaL_loadstring(l, line);
			if (strload == 0) {
				int res = lua_pcall(l, 0, LUA_MULTRET, 0);
				plua_error(l, res, "Input");
			} else {
				plua_error(l, strload, "Input");
			}
		}
		line[0] = 'p';
		line[1] = 'r';
		line[2] = 'i';
		line[3] = 'n';
		line[4] = 't';
		line[5] = '(';
		cmdlen = read_command_tobuf(fd, line + 6, 2048 - 6) + 6;
	} while(cmdlen > 0);
}

void lua_debughook(lua_State* l, lua_Debug* d)
{
	if (lua_getinfo(l, "nSlu", d) > 0) {
		write_debug(1, d);
		read_command(2, l);
	}
}

void setup_debug(lua_State* l)
{
	//lua_sethook(l, &lua_debughook, LUA_MASKCOUNT, 1);
}

