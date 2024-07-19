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

bool XAudioDriver::InitializeXaudio() {
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// Initialize COM
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

	// Initialize XAudio
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

	// Initialize XAudio mastering voice
	hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);

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

	else if (fileP == ERROR_NEGATIVE_SEEK) {
		// Print error message
		hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SetFilePointerEx Error: ERROR_NEGATIVE_SEEK\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());
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
				// Issue right here
				MessageBox(0, L"Failed SetFilePointer for dwChunkType", 0, 0);

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

			else if (fileP == ERROR_NEGATIVE_SEEK) {
				// Print error message
				hr = HRESULT_FROM_WIN32(GetLastError());
				_com_error err(hr);
				LPCTSTR errMsg = err.ErrorMessage();
				OutputDebugStringW(errMsg);
				OutputDebugStringA("\n");
				OutputDebugStringA("SetFilePointerEx Error: ERROR_NEGATIVE_SEEK\n");

				std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
				OutputDebugStringA(debug_msg.c_str());
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

	else if (fileP == ERROR_NEGATIVE_SEEK) {
		// Print error message
		hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SetFilePointerEx Error: ERROR_NEGATIVE_SEEK\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());
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

	else if (fileP == ERROR_NEGATIVE_SEEK) {
		// Print error message
		hr = HRESULT_FROM_WIN32(GetLastError());
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("SetFilePointerEx Error: ERROR_NEGATIVE_SEEK\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());
	}

	DWORD dwChunkSize;
	DWORD dwChunkPosition;

	hr = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);

	if (FAILED(hr)) {
		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("FindChunk Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());
		return false;
	}

	DWORD filetype;

	hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);

	if (FAILED(hr)) {
		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("ReadChunkData Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());
		return false;
	}

	if (filetype != fourccWAVE) {
		MessageBox(0, L"Filetype not WAVE", 0, 0);

		std::string debug_msg = "Filetype not WAVE\n";
		OutputDebugStringA(debug_msg.c_str());

		debug_msg = std::to_string(filetype) + "\n";

		OutputDebugStringA(debug_msg.c_str());
		return false;
	}

	// issue right here
	hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);

	if (FAILED(hr)) {
		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("FindChunk Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	// Locate the 'fmt' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

	if (FAILED(hr)) {
		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("ReadChunkData Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	// issue right here
	// fill out the audio data buffer with the contents of the fourccDATA chunk
	hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);

	if (FAILED(hr)) {
		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("FindChunk Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}
	
	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	
	hr = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	if (FAILED(hr)) {
		// Print error message
		_com_error err(hr);
		LPCTSTR errMsg = err.ErrorMessage();
		OutputDebugStringW(errMsg);
		OutputDebugStringA("\n");
		OutputDebugStringA("ReadChunkData Error\n");

		std::string debug_msg = "Line number: " + std::to_string(__LINE__) + "\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	float playLength = 1.0f;

	// Setup Buffer attributes
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;
	buffer.PlayBegin = 0;
	buffer.PlayLength = UINT32(dwChunkSize * playLength);
	buffer.LoopBegin = buffer.PlayBegin + buffer.PlayLength-1;
	buffer.LoopLength = 0;
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	buffer.pContext = NULL;

	return true;
}

bool XAudioDriver::PlayAudioSound() {
	HRESULT hr;
	IXAudio2SourceVoice* pSourceVoice;
	
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