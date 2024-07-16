#pragma once

/* Third Party Libraries*/
#include <xaudio2.h>
#include <system_error>

class XAudioDriver {
private:
	/* Global Declarations - Audio */
	IXAudio2* pXAudio2 = nullptr;
	IXAudio2MasteringVoice* pMasterVoice = nullptr;

	WAVEFORMATEX wfx = { 0 };
	XAUDIO2_BUFFER buffer = { 0 };
	const TCHAR* audioFilePath = __TEXT(".\\music\\shut_up_fool.wav");
	HANDLE hFile;

	HRESULT hr;
public:
	/* Audio Methods */
	bool InitializeXaudio();
	bool LoadAudioFiles();
	bool PlayAudioSound();
	static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
	void CleanUp();
};
