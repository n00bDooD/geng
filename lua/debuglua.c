#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

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

ssize_t read_command_tobuf(int fd, char* line, size_t len)
{
	size_t linelen = 0;

	while(linelen < len) {
		char c = 0;
		switch(recv(fd, &c, 1, 0)) {
		case 0:
			break;
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
			error("read_command_tobuf");
			// Error
			return -1;
		}
	}
	return linelen;
}

void read_command(int fd, lua_State* l)
{
	char line[2048] = {'\0'};
	ssize_t cmdlen = read_command_tobuf(fd, line, 2048);
	do {
		if (cmdlen > 0) {
			if (cmdlen >= 4 
			    && line[0] == '-'
			    && line[1] == '-'
			    && line[2] == '!') {
				switch(line[3]) {
					case 'c':
						write(fd, "continuing\n", 11);
						cmdlen = -1;
						break;
					default:
						write(fd, "unknown command\n", 16);
				}
			} else {
				int strload = luaL_loadstring(l, line);
				if (strload == 0) {
					int res = lua_pcall(l, 0, LUA_MULTRET, 0);
					if (res == 0) {
						// Command OK, no error info
						write(fd, "\n", 1);
					} else {
						size_t errlen = 0;
						const char* lua_error = lua_tolstring(l, -1, &errlen);
						write(fd, lua_error, errlen);
						write(fd, "\n", 1);
					}
				} else {
					size_t errlen = 0;
					const char* lua_error = lua_tolstring(l, -1, &errlen);
					write(fd, lua_error, errlen);
					write(fd, "\n", 1);
				}
			}
		}
		cmdlen = cmdlen < 0 ? cmdlen : read_command_tobuf(fd, line, 2048);
	} while(cmdlen > 0);
}

static int debug_socket = -1;
static int clientfd = -1;

void lua_debughook(lua_State* l, lua_Debug* d)
{
	if (debug_socket <= 0) return;
	if (lua_getinfo(l, "nSlu", d) > 0) {
		if (clientfd <= 0) {
			struct sockaddr_in client;
			size_t c = sizeof(struct sockaddr_in);
			clientfd = accept(debug_socket, (struct sockaddr*)&client, (socklen_t*)&c);
			if (clientfd < 0) {
				error("accept()");
			}
		}

		if (clientfd >= 0) write_debug(clientfd, d);
		read_command(clientfd, l);
	}
}

void setup_debug(lua_State* l)
{
	if (debug_socket <= 0) {
		debug_socket = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in serv;
		serv.sin_family = AF_INET;
		serv.sin_addr.s_addr = INADDR_ANY;
		serv.sin_port = htons(4321);

		if (bind(debug_socket, (struct sockaddr*)&serv, sizeof(serv)) < 0) {
			error("setup_debug");
		}

		listen(debug_socket, 3);
	}

	lua_sethook(l, &lua_debughook, LUA_MASKCOUNT, 1);
}

