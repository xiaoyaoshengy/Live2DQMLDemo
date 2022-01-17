﻿#include "CubismOffscreenSurface_OpenGLES2.hpp"

namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {
				CubismOffscreenFrame_OpenGLES2::CubismOffscreenFrame_OpenGLES2(QOpenGLContext* context)
					: _renderTexture(0)
					, _colorBuffer(0)
					, _oldFBO(0)
					, _bufferWidth(0)
					, _bufferHeight(0)
					, _isColorBufferInherited(false)
					, _context(context)
				{
				}

				void CubismOffscreenFrame_OpenGLES2::BeginDraw(GLint restoreFBO)
				{
					if (_renderTexture == 0)
					{
						return;
					}

					QOpenGLFunctions* f = _context->functions();

					if (restoreFBO < 0)
					{
						f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
					}
					else
					{
						_oldFBO = restoreFBO;
					}

					f->glBindFramebuffer(GL_FRAMEBUFFER, _renderTexture);
				}

				void CubismOffscreenFrame_OpenGLES2::EndDraw()
				{
					if (_renderTexture == 0)
					{
						return;
					}

					QOpenGLFunctions* f = _context->functions();

					f->glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);
				}

				void CubismOffscreenFrame_OpenGLES2::Clear(float r, float g, float b, float)
				{
					QOpenGLFunctions* f = _context->functions();

					f->glClearColor(r, g, b, 0.0f);
					f->glClear(GL_COLOR_BUFFER_BIT);
				}

				csmBool CubismOffscreenFrame_OpenGLES2::CreateOffscreenFrame(csmUint32 displayBufferWidth, csmUint32 displayBufferHeight, GLuint colorBuffer)
				{
					DestroyOffscreenFrame();

					do
					{
						GLuint ret = 0;

						QOpenGLFunctions* f = _context->functions();

						if (colorBuffer == 0)
						{
							f->glGenTextures(1, &_colorBuffer);

							f->glBindTexture(GL_TEXTURE_2D, _colorBuffer);
							f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, displayBufferWidth, displayBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
							f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							f->glBindTexture(GL_TEXTURE_2D, 0);

							_isColorBufferInherited = false;
						}
						else
						{
							_colorBuffer = colorBuffer;

							_isColorBufferInherited = true;
						}

						GLint tmpFramebufferObject;
						f->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &tmpFramebufferObject);

						f->glGenFramebuffers(1, &ret);
						f->glBindFramebuffer(GL_FRAMEBUFFER, ret);
						f->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer, 0);
						f->glBindFramebuffer(GL_FRAMEBUFFER, tmpFramebufferObject);

						_renderTexture = ret;

						_bufferWidth = displayBufferWidth;
						_bufferHeight = displayBufferHeight;

						return true;
					} while (0);

					DestroyOffscreenFrame();

					return false;
				}

				void CubismOffscreenFrame_OpenGLES2::DestroyOffscreenFrame()
				{
					QOpenGLFunctions* f = _context->functions();

					if (!_isColorBufferInherited && (_colorBuffer != 0))
					{
						f->glDeleteTextures(1, &_colorBuffer);
						_colorBuffer = 0;
					}

					if (_renderTexture != 0)
					{
						f->glDeleteFramebuffers(1, &_renderTexture);
						_renderTexture = 0;
					}
				}
				GLuint CubismOffscreenFrame_OpenGLES2::GetColorBuffer() const
				{
					return _colorBuffer;
				}
				csmUint32 CubismOffscreenFrame_OpenGLES2::GetBufferWidth() const
				{
					return _bufferWidth;
				}
				csmUint32 CubismOffscreenFrame_OpenGLES2::GetBufferHeight() const
				{
					return _bufferHeight;
				}
				csmBool CubismOffscreenFrame_OpenGLES2::IsValid() const
				{
					return _renderTexture != 0;
				}
} } } }

