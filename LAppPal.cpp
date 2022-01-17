#include "LAppPal.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <fstream>
#include "CubismSDK/Framework/Model/CubismMoc.hpp"
#include "LAppDefine.h"
#include <QDebug>
#include <QString>
#include <QDateTime>

using namespace Csm;
using namespace std;
using namespace LAppDefine;

double LAppPal::s_currentFrame = 0.0;
double LAppPal::s_lastFrame = 0.0;
double LAppPal::s_deltaTime = 0.0;

Csm::csmByte* LAppPal::LoadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize)
{
	const char* path = filePath.c_str();

	int size = 0;
	struct stat statBuf;
	if (stat(path, &statBuf) == 0)
	{
		size = statBuf.st_size;
	}

	std::fstream file;
	char* buf = new char[size];

	file.open(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		if (DebugLogEnable)
		{
			PrintLog("file open error");
		}
		delete[] buf;
		return NULL;
	}
	file.read(buf, size);
	file.close();

	*outSize = size;
	return reinterpret_cast<csmByte*>(buf);
}

void LAppPal::ReleaseBytes(Csm::csmByte* byteData)
{
	delete[] byteData;
}

Csm::csmFloat32 LAppPal::GetDeltaTime()
{
	return static_cast<csmFloat32>(s_deltaTime);
}

void LAppPal::UpdateTime()
{
	s_currentFrame = static_cast<double>(QDateTime::currentMSecsSinceEpoch()) / 1000.0;
	s_deltaTime = s_currentFrame - s_lastFrame;
	s_lastFrame = s_currentFrame;
}

void LAppPal::PrintLog(const Csm::csmChar* format, ...)
{
	va_list args;
	va_start(args, format);
	qDebug().noquote() << QString::vasprintf(format, args) << Qt::endl;
	va_end(args);
}

void LAppPal::PrintMessage(const Csm::csmChar* message)
{
	PrintLog("%s", message);
}
