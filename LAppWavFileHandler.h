#pragma once

#include "Framework/CubismFramework.hpp"
#include "Framework/Utils/CubismString.hpp"

class LAppWavFileHandler
{
public:
	LAppWavFileHandler();
	~LAppWavFileHandler();
	Csm::csmBool Update(Csm::csmFloat32 deltaTimeSeconds);
	bool Start(const Csm::csmString& filePath);
	Csm::csmFloat32 GetRms() const;

private:
	Csm::csmBool LoadWavFile(const Csm::csmString& filePath);
	void ReleasePcmData();
	Csm::csmFloat32 GetPcmSample();
	
	struct WavFileInfo
	{
		WavFileInfo()
			: _fileName("")
			, _numberOfChannels(0)
			, _bitsPerSample(0)
			, _samplingRate(0)
			, _samplesPerChannel(0)
		{ }

		Csm::csmString _fileName;
		Csm::csmUint32 _numberOfChannels;
		Csm::csmUint32 _bitsPerSample;
		Csm::csmUint32 _samplingRate;
		Csm::csmUint32 _samplesPerChannel;
	} _wavFileInfo;

	struct ByteReader
	{
		ByteReader()
			: _fileByte(NULL)
			, _fileSize(0)
			, _readOffset(0)
		{ }

		Csm::csmUint8 Get8()
		{
			const Csm::csmUint8 ret = _fileByte[_readOffset];
			_readOffset++;
			return ret;
		}

		Csm::csmUint16 Get16LittleEndian()
		{
			const Csm::csmUint16 ret = (_fileByte[_readOffset + 1] << 8) | _fileByte[_readOffset];
			_readOffset += 2;
			return ret;
		}

		Csm::csmUint32 Get24LittleEndian()
		{
			const Csm::csmUint32 ret =
				(_fileByte[_readOffset + 2] << 16) | (_fileByte[_readOffset + 1] << 8)
				| _fileByte[_readOffset];
			_readOffset += 3;
			return ret;
		}

		Csm::csmUint32 Get32LittleEndian()
		{
			const Csm::csmUint32 ret =
				(_fileByte[_readOffset + 3] << 24) | (_fileByte[_readOffset + 2] << 16)
				| (_fileByte[_readOffset + 1] << 8) | _fileByte[_readOffset];
			_readOffset += 4;
			return ret;
		}

		Csm::csmBool GetCheckSignature(const Csm::csmString& reference)
		{
			Csm::csmChar getSignature[4] = { 0, 0, 0, 0 };
			const Csm::csmChar* referenceString = reference.GetRawString();
			if (reference.GetLength() != 4)
			{
				return false;
			}
			for (Csm::csmUint32 signatureOffset = 0; signatureOffset < 4; signatureOffset++)
			{
				getSignature[signatureOffset] = static_cast<Csm::csmChar>(Get8());
			}
			return (getSignature[0] == referenceString[0]) && (getSignature[1] == referenceString[1])
				&& (getSignature[2] == referenceString[2]) && (getSignature[3] == referenceString[3]);
		}

		Csm::csmByte* _fileByte;
		Csm::csmSizeInt _fileSize;
		Csm::csmUint32 _readOffset;
	} _byteReader;

	Csm::csmFloat32** _pcmData;
	Csm::csmUint32 _sampleOffset;
	Csm::csmFloat32 _lastRms;
	Csm::csmFloat32 _userTimeSeconds;
};

