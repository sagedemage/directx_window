/* Third Party Libraries*/
#include <comdef.h>

/* Standard Libraries */
#include <iostream>
#include <string>

/* Local header files */
#include "audio.h"
//#include "libstopwatch.h"
#include "hresult_debugger.h"

Audio::Audio(const int channels, const int chunksize) {
	// Open Audio
	int status = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, channels, chunksize);

	if (status == -1) {
		std::string debug_msg = "Mix_OpenAudio: " + (std::string)Mix_GetError() + "\n";
		OutputDebugStringA(debug_msg.c_str());
	}
}

bool Audio::loadMusic(const char* music_path) {
	/* Load music */
	music = Mix_LoadMUS(music_path);

	if (music == NULL) {
		std::string debug_msg = "Mix_LoadMUS: " + (std::string)Mix_GetError() + "\n";
		OutputDebugStringA(debug_msg.c_str());
		return false;
	}
	return true;
}

void Audio::playMusic() {
	/* Play music */
	int status = Mix_PlayMusic(music, -1);

	if (status == -1) {
		std::string debug_msg = "Mix_PlayMusic: " + (std::string)Mix_GetError() + "\n";
		OutputDebugStringA(debug_msg.c_str());
	}
}

void Audio::changeVolume(const int music_volume) {
	/* Change music volume */
	Mix_VolumeMusic(music_volume);
}

void Audio::freeResources() {
	/* Free resources */
	Mix_FreeMusic(music);
	Mix_CloseAudio();
}