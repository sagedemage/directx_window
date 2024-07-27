#ifndef AUDIO_H
#define AUDIO_H

/* SDL_mixer Library */
#include <SDL.h>
#include <SDL_mixer.h>

/* Standard Libraries */
#include <iostream>
#include <string>

class Audio {
public:
	Audio(const int channels, const int chunksize);

	// Methods
	bool loadMusic(const char* music_path);
	void playMusic();
	static void changeVolume(const int music_volume);
	void freeResources();

private:
	Mix_Music* music;

};
#endif // AUDIO_H
