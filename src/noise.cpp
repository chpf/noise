#include "noise.hpp"

// log string, and float
#define LOG(s,x)  {   					                            \
					char msgbuf[100]  = {};                         \
					size_t msgsize = 100;                           \
					sprintf_s(msgbuf, msgsize, "%s %f\n",(s), (x)); \
					OutputDebugString(msgbuf);                      \
				}                                                   \


template<typename T>
T map_range(T input , T output_start, T output_end, T input_start, T input_end) {
	return output_start + ((output_end - output_start) / (input_end - input_start)) * (input - input_start);
}

//TODO cpp has constructors
HRESULT Noise::init(HWND& hwnd) {
	HRESULT hr;
	hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
	if (FAILED(hr)) {
		OutputDebugString(L"Could not Initialize COM");
    	return hr;
	}

	// .WAV format 44100 samples per second in 16-bit stereo 
	// aka 176400 bytes
	this->wavFormat = {
		.wFormatTag = WAVE_FORMAT_PCM,
		.nChannels = 2,
		.nSamplesPerSec = 44100,
		.nAvgBytesPerSec = 176400,
		.nBlockAlign = 4,
		.wBitsPerSample = 16,
		.cbSize = 0,
	}; 

	// fill audio buffer with something
	// format reference
		//http://soundfile.sapp.org/doc/WaveFormat/
	int counter = 0;
	int hz = 200;
	int volume = 4000;
	int segment_len = 88200/hz;
	auto pData = new int16_t[88200]{};
	for(int i = 0; i < 88200 -2 ; i+= 2) {
		if ((i % (segment_len*2)) > segment_len) {
			pData[i] = -1; //left
			pData[i+1] = -1; //right
		} else {
			pData[i] = 1;
			pData[i+1] = 1;
		}
		pData[i]*= volume;
		pData[i+1]*= volume;

	}

	this->audio_data.data = (BYTE*)pData;
	this->audio_data.size = 176400;

	XAUDIO2_BUFFER  xaudio2 = {
		.Flags = 0,
		.AudioBytes = this->audio_data.size, // size of audio data in bytes ?????
		.pAudioData = this->audio_data.data, // pointer to that data
		.PlayBegin = 0, // first sample
		.PlayLength = 0,  // entire buffer
		.LoopBegin = 0,
		.LoopLength = 0,
		//.LoopCount = 0, // no loop
		.LoopCount = XAUDIO2_LOOP_INFINITE,
		.pContext = NULL,
   };

	if ( FAILED(hr = XAudio2Create( &this->pXAudio2Engine, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) ) {
		OutputDebugString(L"XAudio2Create failed\n");
    	return hr;
	}
	if ( FAILED(hr = this->pXAudio2Engine->CreateMasteringVoice( 
						&this->pMasterVoice,
						XAUDIO2_DEFAULT_CHANNELS,
						44100,  // samplerate
						NULL,
						NULL))) {
		OutputDebugString(L"CreateMasteringVoice failed\n");
	    return hr;
	}
	if( FAILED(hr = this->pXAudio2Engine->CreateSourceVoice(
						&this->pSourceVoice,
						(WAVEFORMATEX*)&wavFormat,
						0u,
   						1.0f))) {
		OutputDebugString(L"CreateSourceVoice failed\n");
		return hr;
	}
	if( FAILED(hr = this->pSourceVoice->SubmitSourceBuffer(&xaudio2))) {
    	return hr;
	}
	return S_OK;
}

void Noise::play() {
	this->pSourceVoice->Start(0);
}

void Noise::pause() {
	this->pSourceVoice->Stop(0);
}


Noise::~Noise() {
	delete this->audio_data.data;
	pXAudio2Engine->Release();
	CoUninitialize();
}