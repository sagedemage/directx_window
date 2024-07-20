/* Standard Libraries */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <comdef.h>
#include <exception>

/* Local header files */
#include "xaudio_driver.h"
#include "xaudio.h"

bool XAudioDriver::InitializeXaudio(float volume) {
	/* Initialize COM Library */
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed CoInitializeEx", 0, 0);

		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("CoInitializeEx Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	/* Initialize XAudio to create an instance of the XAudio2 engine */
	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed XAudio2Create", 0, 0);

		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("XAudio2Create Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	/* Create a mastering voice */
	hr = pXAudio2->CreateMasteringVoice(
		&pMasterVoice, 
		XAUDIO2_DEFAULT_CHANNELS, 
		XAUDIO2_DEFAULT_SAMPLERATE,
		0,
		NULL,
		NULL
		);

	pMasterVoice->SetVolume(volume);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed CreateMasteringVoice", 0, 0);

		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("CreateMasteringVoice Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	return true;
}

HRESULT XAudioDriver::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition) {
	HRESULT hr = S_OK;

	DWORD fileP = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	if (fileP == INVALID_SET_FILE_POINTER) {
		// Print error message
		hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SetFilePointerEx Error: INVALID_SET_FILE_POINTER\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return hr;
	}

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK) {
		DWORD dwRead;
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		// fourccDATA
		switch (dwChunkType) {
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0) {
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;
		default:
			// the dwChunkDataSize is not the right parameter
			DWORD fileP = SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT);
			if (fileP == INVALID_SET_FILE_POINTER) {
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc) {
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) {
			return S_FALSE;
		}
	}

	return S_OK;
}

HRESULT XAudioDriver::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;

	DWORD fileP = SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN);

	if (fileP == INVALID_SET_FILE_POINTER) {
		// Print error message
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SetFilePointer Error: INVALID_SET_FILE_POINTER\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return hr;
	}

	DWORD dwRead;
	if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL) == 0) {
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	return hr;
}

bool XAudioDriver::LoadAudioFile(LPCSTR audioFilePath) {
	HRESULT hr;
	HANDLE hFile = CreateFileA(audioFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(0, L"Failed CreateFile", 0, 0);

		// Print error message
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("CreateFileA Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	DWORD fileP = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	if (fileP == INVALID_SET_FILE_POINTER) {
		MessageBox(0, L"Failed SetFilePointer", 0, 0);

		// Print error message
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SetFilePointerEx Error: INVALID_SET_FILE_POINTER\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	
	/* RIFF Chunk */
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);

	if (filetype != fourccWAVE) {
		MessageBox(0, L"Filetype not WAVE", 0, 0);

		std::string debug_msg = "Filetype not WAVE\n";
		OutputDebugStringA(debug_msg.c_str());

		debug_msg = std::to_string(filetype) + "\n";

		OutputDebugStringA(debug_msg.c_str());
		return false;
	}

	/* FMT Chunk */
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	// Locate the 'fmt' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

	/* Data chunk */
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	float playLength = 1.0f;

	// Setup Buffer attributes
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;
	
	buffer.PlayBegin = 0;
	buffer.PlayLength = UINT32(dwChunkSize * playLength);
	buffer.LoopBegin = buffer.PlayBegin + buffer.PlayLength-1;
	buffer.LoopLength = 0;
	buffer.LoopCount = 1;
	buffer.pContext = NULL;

	return true;
}

bool XAudioDriver::PlayAudioSound() {
	HRESULT hr;
	IXAudio2SourceVoice* pSourceVoice;

	/*
	  8.000 kHz =  8000L
	 11.025 kHz = 11025L
	 22.050 kHz = 22050L
	 44.100 kHz = 44100L
	 */
	const long nSamplesPerSec = 11025L;
	const int nChannels = 1;
	const int wBitsPerSample = 8;
	const int nBlockAlign = (nChannels*wBitsPerSample)/8;

	wfx.Format.wFormatTag = WAVE_FORMAT_PCM;
	wfx.Format.nChannels = nChannels;
	wfx.Format.nSamplesPerSec = nSamplesPerSec;
	wfx.Format.nAvgBytesPerSec = nSamplesPerSec * nBlockAlign;
	wfx.Format.nBlockAlign = nBlockAlign;
	wfx.Format.wBitsPerSample = wBitsPerSample;
	wfx.Format.cbSize = 22;

	wfx.Samples.wValidBitsPerSample = 8;
	wfx.Samples.wSamplesPerBlock = 0;
	wfx.Samples.wReserved = 0;
	
	// Initialize XAudio source voice
	hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);

	if (FAILED(hr)) {
		std::string error_code;

		MessageBox(0, L"Failed CreateSourceVoice", 0, 0);

		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");

		std::string errCode = "Error code: " + std::to_string(err.WCode());
		OutputDebugStringA(errCode.c_str());
		OutputDebugStringA("\n");
		OutputDebugStringA("CreateSourceVoice Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	// Adds a new audio buffer to the voice queue
	hr = pSourceVoice->SubmitSourceBuffer(&buffer);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed SubmitSourceBuffer", 0, 0);

		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SubmitSourceBuffer Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	// Set frequency of the source voice
	float sourceRate = 1;
	float targetRate = 32;
	float frequencyRatio = sourceRate / targetRate;
	pSourceVoice->SetFrequencyRatio(frequencyRatio);

	// Start the XAudio source voice
	hr = pSourceVoice->Start(0);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed Start", 0, 0);

		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("Start Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	return true;
}

void XAudioDriver::CleanUp() {
	pXAudio2->Release();
}