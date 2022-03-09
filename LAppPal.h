#pragma once

#include "Framework/CubismFramework.hpp"
#include <string>

class LAppPal
{
public:
	static Csm::csmByte* LoadFileAsBytes(const std::string filePath, Csm::csmSizeInt* outSize);
	static void ReleaseBytes(Csm::csmByte* byteData);
	static Csm::csmFloat32 GetDeltaTime();
	static void UpdateTime();
	static void PrintLog(const Csm::csmChar* format, ...);
	static void PrintMessage(const Csm::csmChar* message);

private:
	static double s_currentFrame;
	static double s_lastFrame;
	static double s_deltaTime;
};

