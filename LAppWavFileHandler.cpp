#include "LAppWavFileHandler.h"
#include <cmath>
#include <cstdint>
#include "LAppPal.h"

LAppWavFileHandler::LAppWavFileHandler()
	: _pcmData(NULL)
	, _userTimeSeconds(0.0f)
	, _lastRms(0.0f)
	, _sampleOffset(0)
{
}

LAppWavFileHandler::~LAppWavFileHandler()
{
	if (_pcmData != NULL)
	{
		ReleasePcmData();
	}
}

Csm::csmBool LAppWavFileHandler::Update(Csm::csmFloat32 deltaTimeSeconds)
{
	Csm::csmUint32 goalOffset;
	Csm::csmFloat32 rms;

	if ((_pcmData == NULL) || (_sampleOffset >= _wavFileInfo._samplesPerChannel))
	{
		_lastRms = 0.0f;
		return false;
	}

	_userTimeSeconds += deltaTimeSeconds;
	goalOffset = static_cast<Csm::csmUint32>(_userTimeSeconds * _wavFileInfo._samplingRate);
	if (goalOffset > _wavFileInfo._samplesPerChannel)
	{
		goalOffset = _wavFileInfo._samplesPerChannel;
	}

	rms = 0.0f;
	for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
	{
		for (Csm::csmUint32 sampleCount = _sampleOffset; sampleCount < goalOffset; sampleCount++)
		{
			Csm::csmFloat32 pcm = _pcmData[channelCount][sampleCount];
			rms += pcm * pcm;
		}
	}
	rms = sqrt(rms / (_wavFileInfo._numberOfChannels * (goalOffset - _sampleOffset)));

	_lastRms = rms;
	_sampleOffset = goalOffset;
	return true;
}

bool LAppWavFileHandler::Start(const Csm::csmString& filePath)
{
	if (!LoadWavFile(filePath))
	{
		return false;
	}

	_sampleOffset = 0;
	_userTimeSeconds = 0.0f;

	_lastRms = 0.0f;
	return true;
}

Csm::csmFloat32 LAppWavFileHandler::GetRms() const
{
	return _lastRms;
}

Csm::csmBool LAppWavFileHandler::LoadWavFile(const Csm::csmString& filePath)
{
	Csm::csmBool ret;

	if (_pcmData != NULL)
	{
		ReleasePcmData();
	}

	_byteReader._fileByte = LAppPal::LoadFileAsBytes(filePath.GetRawString(), &(_byteReader._fileSize));
	_byteReader._readOffset = 0;

	if ((_byteReader._fileByte == NULL) || (_byteReader._fileSize < 4))
	{
		return false;
	}

	_wavFileInfo._fileName = filePath;

	do
	{
		if (!_byteReader.GetCheckSignature("RIFF"))
		{
			ret = false;
			break;
		}

		_byteReader.Get32LittleEndian();

		if (!_byteReader.GetCheckSignature("WAVE"))
		{
			ret = false;
			break;
		}

		if (!_byteReader.GetCheckSignature("fmt "))
		{
			ret = false;
			break;
		}

		const Csm::csmUint32 fmtChunkSize = _byteReader.Get32LittleEndian();

		if (_byteReader.Get16LittleEndian() != 1)
		{
			ret = false;
			break;
		}

		_wavFileInfo._numberOfChannels = _byteReader.Get16LittleEndian();

		_wavFileInfo._samplingRate = _byteReader.Get32LittleEndian();

		_byteReader.Get32LittleEndian();

		_byteReader.Get16LittleEndian();

		_wavFileInfo._bitsPerSample = _byteReader.Get16LittleEndian();

		if (fmtChunkSize > 16)
		{
			_byteReader._readOffset += (fmtChunkSize - 16);
		}

		while (!(_byteReader.GetCheckSignature("data")) && (_byteReader._readOffset < _byteReader._fileSize))
		{
			_byteReader._readOffset += _byteReader.Get32LittleEndian();
		}

		if (_byteReader._readOffset >= _byteReader._fileSize)
		{
			ret = false;
			break;
		}

		{
			const Csm::csmUint32 dataChunkSize = _byteReader.Get32LittleEndian();
			_wavFileInfo._samplesPerChannel = (dataChunkSize * 8) / (_wavFileInfo._bitsPerSample * _wavFileInfo._numberOfChannels);
		}

		_pcmData = static_cast<Csm::csmFloat32**>(CSM_MALLOC(sizeof(Csm::csmFloat32*) * _wavFileInfo._numberOfChannels));
		for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
		{
			_pcmData[channelCount] = static_cast<Csm::csmFloat32*>(CSM_MALLOC(sizeof(Csm::csmFloat32) * _wavFileInfo._samplesPerChannel));
		}

		for (Csm::csmUint32 sampleCount = 0; sampleCount < _wavFileInfo._samplesPerChannel; sampleCount++)
		{
			for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
			{
				_pcmData[channelCount][sampleCount] = GetPcmSample();
			}
		}

		ret = true;
	} while (false);

	LAppPal::ReleaseBytes(_byteReader._fileByte);
	_byteReader._fileByte = NULL;
	_byteReader._fileSize = 0;

	return ret;
}

void LAppWavFileHandler::ReleasePcmData()
{
	for (Csm::csmUint32 channelCount = 0; channelCount < _wavFileInfo._numberOfChannels; channelCount++)
	{
		CSM_FREE(_pcmData[channelCount]);
	}
	CSM_FREE(_pcmData);
	_pcmData = NULL;
}

Csm::csmFloat32 LAppWavFileHandler::GetPcmSample()
{
	Csm::csmInt32 pcm32;

	switch (_wavFileInfo._bitsPerSample)
	{
	case 8:
		pcm32 = static_cast<Csm::csmInt32>(_byteReader.Get8()) - 128;
		pcm32 <<= 24;
		break;
	case 16:
		pcm32 = _byteReader.Get16LittleEndian() << 16;
		break;
	case 24:
		pcm32 = _byteReader.Get24LittleEndian() << 8;
		break;
	default:
		pcm32 = 0;
		break;
	}

	return static_cast<Csm::csmFloat32>(pcm32) / INT32_MAX;
}
