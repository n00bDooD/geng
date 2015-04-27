#ifndef SDLAUDIO_H
#define SDLAUDIO_H

#include <SDL2/SDL_mixer.h>

typedef struct {
	Mix_Chunk** chunks;
	Mix_Music** musics;
} sdl_audio;

#endif /* SDLAUDIO_H */
