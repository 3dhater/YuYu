#ifndef _YUYU_AUDIO_H_
#define _YUYU_AUDIO_H_

#include "containers\array.h"
#include "strings\string.h"

class yyAudioSource;

// Реализация работы для конкретного формата аудио
struct yyAudioSourceFileLoader
{
	yyAudioSourceFileLoader(){
		Load = nullptr;
	}
	virtual ~yyAudioSourceFileLoader() {}
	yyArraySmall<yyStringA> m_extensions;
	yyAudioSource* (*Load)(const char*);
};
struct yyAudioSourceFileLoaderCollection
{
	yyArraySmall<yyAudioSourceFileLoader*> m_loaders;

	yyAudioSourceFileLoaderCollection() {}
	~yyAudioSourceFileLoaderCollection()
	{
		for (u16 i = 0, sz = m_loaders.size(); i < sz; ++i)
		{
			yyDestroy(m_loaders[i]);
		}
	}
};
struct yyAudioSourceInfo
{
	yyAudioSourceInfo()
	{
		m_formatType = 0;	//< for XAudio2 - 1 - PCM
		m_channels = 0;		//< number of channels (i.e. mono, stereo...)
		m_sampleRate = 0;	//< 44100 et.c.
		m_bytesPerSec = 0;	//<
		m_blockAlign = 0;	//<
		m_bitsPerSample = 0;//< 16bits
		m_dataSize = 0;
	}
	u32		m_formatType;
	u32		m_channels;
	u32		m_sampleRate;
	u32		m_bytesPerSec;
	u32		m_blockAlign;
	u32		m_bitsPerSample;
	u32     m_dataSize;
};
class yyAudioSource
{
public:

	yyAudioSource() {
		m_data = nullptr;     //Звук
		m_dataSize = 0; //Размер в байтах
		m_implementation = nullptr;
	}
	~yyAudioSource()
	{
		if (m_data)
			yyMemFree(m_data);
		if (m_implementation)
			yyDestroy(m_implementation);
	}

	yyAudioSourceInfo m_info;

	u8	*	m_data;     //Звук
	u32		m_dataSize; //Размер в байтах

							// данные зависимые от типа аудиодрайвера
	void* m_implementation;
};

// Состояния аудио объекта
enum class yyAudioState : u32
{
	Play,   // Когда звук играет
	Pause,  // Когда воспроизведение приостановлено
	Stop    // Когда звук не воспроизводился или остановлен
};

class yyAudioObject
{
protected:
	bool			m_isLoop;
	yyAudioState	m_state;
	f64				m_time;
	//yyStringA	m_name;
	yyAudioSource* m_audioSource;
	bool m_autoSourceDelete;
	f32 m_volume;
	f32 m_pitch;
public:
	yyAudioObject() {
		m_isLoop = false;
		m_state = yyAudioState::Stop;
		m_time = 0.f;
		m_audioSource = nullptr;
		m_autoSourceDelete = false;
		m_volume = 1.f;
		m_pitch = 1.f;
	}
	virtual ~yyAudioObject()
	{
		if (m_autoSourceDelete && m_audioSource)
		{
			yyDestroy(m_audioSource);
		}
	}
	bool IsPlay() { return m_state == yyAudioState::Play; }
	bool IsPause() { return m_state == yyAudioState::Pause; }
	bool IsStop() { return m_state == yyAudioState::Stop; }
	bool IsLoop() { return m_isLoop; }
	//const yyStringA& GetName(){ return m_name; }
	//void SetName( const yyStringA& name ){ m_name = name; }
	f64 GetTime() { return m_time; }
	f32  GetVolume() { return m_volume; };

	yyAudioSource* GetAudioSource()
	{
		return m_audioSource;
	};

	f32 GetPitch() { return m_pitch; }
	virtual void SetPitch(f32) = 0;

	virtual void Pause() = 0;
	virtual void Play() = 0;
	virtual void SetLoop(bool isLoop) = 0;
	virtual void SetVolume(f32 volume) = 0;
	virtual void Stop() = 0;
};

// Класс для воспроизведения потокового аудио
class yyAudioStream : public yyAudioObject
{
public:
	yyAudioStream() {}
	virtual ~yyAudioStream() {}

	//	Close stream
	virtual bool	Close() = 0;
	// Получить текущую позицию воспроизведения
	virtual f32		GetPlaybackPosition() = 0;
	// Open file for streaming
	virtual bool	Open(const yyString& fileName) = 0;
	// Установить позицию воспроизведения
	virtual void	SetPlaybackPosition(f32 position) = 0;
};

//yyAudioSystem
class yyAudioSystem
{
protected:
	yyAudioSourceFileLoaderCollection* m_loadersPtr;
public:
	yyAudioSystem() {
		m_loadersPtr = nullptr;
	}
	virtual ~yyAudioSystem() {}

	virtual bool Init() = 0;

	virtual yyAudioObject*	CreateAudioObject(const char* fileName, u32 sp = 1u, void(*callback)() = nullptr) = 0;
	virtual yyAudioObject*	CreateAudioObject(yyAudioSource* source, bool autoSourceDelete, u32 sp = 1u, void(*callback)() = nullptr) = 0;
	virtual yyAudioStream*	CreateStream(const char* fileName) = 0;
	virtual const s8*			GetSupportedExtension(u32 id) = 0;
	virtual u32					GetSupportedExtensionCount() = 0;
	virtual yyAudioSource*	LoadAudioSource(const char* fileName) = 0;

};


#endif