#pragma once

#include "CubismSDK/Framework/CubismFramework.hpp"

namespace LAppDefine
{
	using namespace Csm;

	extern const csmFloat32 ViewScale;
	extern const csmFloat32 ViewMaxScale;
	extern const csmFloat32 ViewMinScale;

	extern const csmFloat32 ViewLogicalLeft;
	extern const csmFloat32 ViewLogicalRight;
	extern const csmFloat32 ViewLogicalBottom;
	extern const csmFloat32 ViewLogicalTop;

	extern const csmFloat32 ViewLogicalMaxLeft;
	extern const csmFloat32 ViewLogicalMaxRight;
	extern const csmFloat32 ViewLogicalMaxBottom;
	extern const csmFloat32 ViewLogicalMaxTop;

	extern const csmChar* ResourcesPath;
	extern const csmChar* BackImageName;
	extern const csmChar* GearImageName;
	extern const csmChar* PowerImageName;

	extern const csmChar* ModelDir[];
	extern const csmInt32 ModelDirSize;

	extern const csmChar* MotionGroupIdle;
	extern const csmChar* MotionGroupTapBody;

	extern const csmChar* HitAreaNameHead;
	extern const csmChar* HitAreaNameBody;

	extern const csmInt32 PriorityNone;
	extern const csmInt32 PriorityIdle;
	extern const csmInt32 PriorityNormal;
	extern const csmInt32 PriorityForce;

	extern const csmBool DebugLogEnable;
	extern const csmBool DebugTouchLogEnable;

	extern const CubismFramework::Option::LogLevel CubismLoggingLevel;

	extern const csmInt32 RenderTargetWidth;
	extern const csmInt32 RenderTargetHeight;
}