#pragma once
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <xaudio2.h>
#include <vector>
#include <array>
#include <cmath>

struct Audio_Data {
	BYTE *data;
	uint32_t size;
};

class Noise {
    public:
	HRESULT init(HWND &hwnd);
	void play();
	void pause();
	~Noise();

    private:
	IXAudio2 *pXAudio2Engine = nullptr;
	IXAudio2MasteringVoice *pMasterVoice = nullptr;
	IXAudio2SourceVoice *pSourceVoice = nullptr;
	WAVEFORMATEX wavFormat = {};
	Audio_Data audio_data = {};
};