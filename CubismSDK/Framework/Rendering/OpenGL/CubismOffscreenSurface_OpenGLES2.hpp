/**
 * @author xiaoyaosheny
 * @date 2022/1/10
 * @description 使用Qt对渲染部分进行重写
 */

#pragma once

#include "../../CubismFramework.hpp"
#include "../../Type/csmVector.hpp"
#include "../../Type/csmRectF.hpp"
#include "../../Type/csmMap.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLContext>

//--------------- LIVE2D NAMESPACE --------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {
				
				class CubismOffscreenFrame_OpenGLES2
				{
				public:
					CubismOffscreenFrame_OpenGLES2(QOpenGLContext* context);
					void BeginDraw(GLint restoreFBO = -1);
					void EndDraw();
					void Clear(float r, float g, float b, float a);
					csmBool CreateOffscreenFrame(csmUint32 displayBufferWidth, csmUint32 displayBufferHeight, GLuint colorBuffer = 0);
					void DestroyOffscreenFrame();
					GLuint GetColorBuffer() const;
					csmUint32 GetBufferWidth() const;
					csmUint32 GetBufferHeight() const;
					csmBool IsValid() const;

				private:
					GLuint		_renderTexture;				///< 作为渲染目标的地址
					GLuint		_colorBuffer;				///< 绘图时使用的纹理地址
					GLint		_oldFBO;					///< 旧的FBO
					csmUint32	_bufferWidth;				///< Create时指定的宽度
					csmUint32	_bufferHeight;				///< Create时指定的高度
					csmBool		_isColorBufferInherited;	///< 是否继承纹理缓冲

					QOpenGLContext* _context;				///< OpenGL渲染上下文
				};
} } } }