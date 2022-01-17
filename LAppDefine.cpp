#include "LAppDefine.h"
#include "CubismSDK/Framework/CubismFramework.hpp"

namespace LAppDefine
{
	using namespace Csm;

	const csmFloat32 ViewScale = 1.0f;
	const csmFloat32 ViewMaxScale = 2.0f;
	const csmFloat32 ViewMinScale = 0.8f;

	const csmFloat32 ViewLogicalLeft = -1.0f;
	const csmFloat32 ViewLogicalRight = 1.0f;
	const csmFloat32 ViewLogicalBottom = -1.0f;
	const csmFloat32 ViewLogicalTop = 1.0f;

	const csmFloat32 ViewLogicalMaxLeft = -2.0f;
	const csmFloat32 ViewLogicalMaxRight = 2.0f;
	const csmFloat32 ViewLogicalMaxBottom = -2.0f;
	const csmFloat32 ViewLogicalMaxTop = 2.0f;

	const csmChar* ResourcesPath = "Resources/";
	const csmChar* BackImageName = "back_class_normal.png";
	const csmChar* GearImageName = "icon_gear.png";
	const csmChar* PowerImageName = "close.png";

	const csmChar* ModelDir[] = 
	{
		"aidang_2",
		"Haru",
		"Hiyori",
		"Mark",
		"Natori",
		"Rice", 
	};
	const csmInt32 ModelDirSize = sizeof(ModelDir) / sizeof(const csmChar*);

	const csmChar* MotionGroupIdle = "Idle";
	const csmChar* MotionGroupTapBody = "TapBody";

	const csmChar* HitAreaNameHead = "Head";
	const csmChar* HitAreaNameBody = "Body";

	const csmInt32 PriorityNone = 0;
	const csmInt32 PriorityIdle = 1;
	const csmInt32 PriorityNormal = 2;
	const csmInt32 PriorityForce = 3;

	const csmBool DebugLogEnable = true;
	const csmBool DebugTouchLogEnable = false;

	const CubismFramework::Option::LogLevel CubismLoggingLevel = CubismFramework::Option::LogLevel_Verbose;

	const csmInt32 RenderTargetWidth = 1900;
	const csmInt32 RenderTargetHeight = 1000;
}