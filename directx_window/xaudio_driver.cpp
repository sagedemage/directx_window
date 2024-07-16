/* Standard Libraries */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

/* Local header files */
#include "xaudio_driver.h"
#include "xaudio.h"

bool XAudioDriver::InitializeXaudio() {
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed CoInitializeEx", 0, 0);

		std::string debug_msg = "CoInitializeEx ERROR\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed XAudio2Create", 0, 0);

		std::string debug_msg = "XAudio2Create ERROR\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed CreateMasteringVoice", 0, 0);

		std::string debug_msg = "CreateMasteringVoice ERROR\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	return true;
}

HRESULT XAudioDriver::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition) {
	HRESULT hr = S_OK;

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK) {
		DWORD dwRead;
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}
		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType) {
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL)) {
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;
		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT)) {
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == fourcc) {
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;
	}

	return S_OK;
}

bool XAudioDriver::LoadAudioFiles() {
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 8.0;
	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.wBitsPerSample = 8;
	wfx.cbSize = 0;

	hFile = CreateFile(audioFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		HRESULT error_code = HRESULT_FROM_WIN32(GetLastError());

		MessageBox(0, L"Failed CreateFile", 0, 0);

		std::string debug_msg = "CreateFile ERROR\n" + error_code;
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	DWORD fileP = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	if (fileP == INVALID_SET_FILE_POINTER) {
		HRESULT error_code = HRESULT_FROM_WIN32(GetLastError());

		MessageBox(0, L"Failed SetFilePointer", 0, 0);

		std::string debug_msg = "SetFilePointer ERROR\n" + error_code;
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	DWORD dwChunkSize;
	DWORD dwChunkPosition;

	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	return true;
}

HRESULT XAudioDriver::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;
	if (SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD dwRead;
	if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL)) {
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	return hr;
}

bool XAudioDriver::PlayAudioSound() {
	IXAudio2SourceVoice* pSourceVoice;
	//hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx);

	const XAUDIO2_VOICE_SENDS pSendList;

	hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, &pSendList, NULL);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed CreateSourceVoice", 0, 0);

		std::string debug_msg = "CreateSourceVoice ERROR\n";
		OutputDebugStringA(debug_msg.c_str());

		std::string message = "ERROR: " + std::system_category().message(hr) + "\n";
		OutputDebugStringA(message.c_str());

		if (pSourceVoice == NULL) {
			OutputDebugStringA("pSourceVoice is NULL");
		}

		switch (hr) {
		case XAUDIO2_E_INVALID_CALL:
			MessageBox(0, L"ERROR CODE: XAUDIO2_E_INVALID_CALL", 0, 0);
			OutputDebugStringA("ERROR CODE: XAUDIO2_E_INVALID_CALL\n");
			break;
		case XAUDIO2_E_XMA_DECODER_ERROR:
			MessageBox(0, L"ERROR CODE: XAUDIO2_E_XMA_DECODER_ERROR", 0, 0);
			OutputDebugStringA("ERROR CODE: XAUDIO2_E_XMA_DECODER_ERROR\n");
			break;
		case XAUDIO2_E_XAPO_CREATION_FAILED:
			MessageBox(0, L"ERROR CODE: XAUDIO2_E_XAPO_CREATION_FAILED", 0, 0);
			OutputDebugStringA("ERROR CODE: XAUDIO2_E_XAPO_CREATION_FAILED\n");
			break;
		case XAUDIO2_E_DEVICE_INVALIDATED:
			MessageBox(0, L"ERROR CODE: XAUDIO2_E_DEVICE_INVALIDATED", 0, 0);
			OutputDebugStringA("ERROR CODE: XAUDIO2_E_DEVICE_INVALIDATED\n");
			break;
		}

		return false;
	}

	hr = pSourceVoice->Start(0);

	if (FAILED(hr)) {
		MessageBox(0, L"Failed Start", 0, 0);

		std::string debug_msg = "Start ERROR\n";
		OutputDebugStringA(debug_msg.c_str());

		return false;
	}

	return true;
}

void XAudioDriver::CleanUp() {
	pXAudio2->Release();
}