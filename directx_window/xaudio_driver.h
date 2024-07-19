#pragma once

/* Third Party Libraries*/
#include <xaudio2.h>
#include <system_error>

class XAudioDriver {
private:
	/* Global Declarations */
	IXAudio2* pXAudio2 = nullptr;
	IXAudio2MasteringVoice* pMasterVoice = nullptr;

	WAVEFORMATEXTENSIBLE wfx = { 0 };
	XAUDIO2_BUFFER buffer = { 0 };

	/* Methods */
	static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
public:
	/* Audio Methods */
	bool InitializeXaudio();
	bool LoadAudioFile(LPCSTR audioFilePath);
	bool PlayAudioSound();
	
	void CleanUp();
};
