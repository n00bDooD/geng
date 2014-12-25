#ifndef SDLAUDIO_H
#define SDLAUDIO_H

#include <SDL/SDL_mixer.h>

typedef struct {
	Mix_Chunk** effects;
	Mix_Music** musics;
} sdl_audio;

#endif /* SDLAUDIO_H */
