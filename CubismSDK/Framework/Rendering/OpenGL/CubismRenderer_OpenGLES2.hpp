/**
 * @author xiaoyaosheny
 * @date 2022/1/10
 * @description 使用Qt对渲染部分进行重写
 */

#pragma once

#include "../CubismRenderer.hpp"
#include "../../CubismFramework.hpp"
#include "CubismOffscreenSurface_OpenGLES2.hpp"
#include "../../Type/csmVector.hpp"
#include "../../Type/csmRectF.hpp"
#include "../../Type/csmMap.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLContext>

//---------------- LIVE2D NAMESPACE ----------------
namespace Live2D {
	namespace Cubism {
		namespace Framework {
			namespace Rendering {
				class CubismRenderer_OpenGLES2;
				class CubismClippingContext;

				class CubismClippingManager_OpenGLES2
				{
					friend class CubismShader_OpenGLES2;
					friend class CubismRenderer_OpenGLES2;

				private:
					CubismRenderer::CubismTextureColor* GetChannelFlagAsColor(csmInt32 channelNo);
					void CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext* clippingContext);
					CubismClippingManager_OpenGLES2(QOpenGLContext* context);
					virtual ~CubismClippingManager_OpenGLES2();
					void Initialize(CubismModel& model, csmInt32 drawableCount, const csmInt32** drawableMasks, const csmInt32* drawableMaskCounts);
					void SetupClippingContext(CubismModel& model, CubismRenderer_OpenGLES2* renderer, GLint lastFBO, GLint lastViewport[4]);
					CubismClippingContext* FindSameClip(const csmInt32* drawableMasks, csmInt32 drawableMaskCounts) const;
					void SetupLayoutBounds(csmInt32 usingClipCount) const;
					csmVector<CubismClippingContext*>* GetClippingContextListForDraw();
					void SetClippingMaskBufferSize(csmInt32 size);
					csmInt32 GetClippingMaskBufferSize() const;

					csmInt32										_currentFrameNo;
					csmVector<CubismRenderer::CubismTextureColor*>	_channelColors;
					csmVector<CubismClippingContext*>				_clippingContextListForMask;
					csmVector<CubismClippingContext*>				_clippingContextListForDraw;
					csmInt32										_clippingMaskBufferSize;
					CubismMatrix44									_tmpMatrix;
					CubismMatrix44									_tmpMatrixForMask;
					CubismMatrix44									_tmpMatrixForDraw;
					csmRectF										_tmpBoundsOnModel;

					QOpenGLContext*									_context;
				};

				class CubismClippingContext
				{
					friend class CubismClippingManager_OpenGLES2;
					friend class CubismShader_OpenGLES2;
					friend class CubismRenderer_OpenGLES2;

				private:
					CubismClippingContext(CubismClippingManager_OpenGLES2* manager, const csmInt32* clippingDrawableIndices, csmInt32 clipCount);
					virtual ~CubismClippingContext();
					void AddClippedDrawable(csmInt32 drawableIndex);
					CubismClippingManager_OpenGLES2* GetClippingManager();

					csmBool								_isUsing;
					const csmInt32*						_clippingIdList;
					csmInt32							_clippingIdCount;
					csmInt32							_layoutChannelNo;
					csmRectF*							_layoutBounds;
					csmRectF*							_allClippedDrawRect;
					CubismMatrix44						_matrixForMask;
					CubismMatrix44						_matrixForDraw;
					csmVector<csmInt32>*				_clippedDrawableIndexList;
					CubismClippingManager_OpenGLES2*	_owner;
				};

				class CubismShader_OpenGLES2
				{
					friend class CubismRenderer_OpenGLES2;

				private:
					static CubismShader_OpenGLES2* GetInstance(QOpenGLContext* context);
					static void DeleteInstance();

					struct CubismShaderSet
					{
						GLuint	ShaderProgram;
						GLuint	AttributePositionLocation;
						GLuint	AttributeTexCoordLocation;
						GLint	UniformMatrixLocation;
						GLint	UniformClipMatrixLocation;
						GLint	SamplerTexture0Location;
						GLint	SamplerTexture1Location;
						GLint	UniformBaseColorLocation;
						GLint	UniformChannelFlagLocation;
					};

					CubismShader_OpenGLES2(QOpenGLContext* context);
					virtual ~CubismShader_OpenGLES2();
					void SetupShaderProgram(
						CubismRenderer_OpenGLES2* renderer,
						GLuint textureId,
						csmInt32 vertexCount,
						csmFloat32* vertexArray,
						csmFloat32* uvArray,
						csmFloat32 opacity,
						CubismRenderer::CubismBlendMode colorBlendMode,
						CubismRenderer::CubismTextureColor baseColor,
						csmBool isPremultipliedAlpha,
						CubismMatrix44 matrix4x4,
						csmBool invertedMask);
					void ReleaseShaderProgram();
					void GenerateShaders();
					GLuint LoadShaderProgram(const csmChar* vertShaderSrc, const csmChar* fragShaderSrc);
					csmBool CompileShaderSource(GLuint* outShader, GLenum shaderType, const csmChar* shaderSource);
					csmBool LinkProgram(GLuint shaderProgram);
					csmBool ValidateProgram(GLuint shaderProgram);

#ifdef CSM_TARGET_ANDROID_ES2
				public:
					static void SetExtShaderMode(csmBool extMode, csmBool extPAMode);

				private:
					static csmBool s_extMode;
					static csmBool s_extPAMode;
#endif // CSM_TARGET_ANDROID_ES2

					csmVector<CubismShaderSet*> _shaderSets;

					QOpenGLContext* _context;
				};

				class CubismRendererProfile_OpenGLES2
				{
					friend class CubismRenderer_OpenGLES2;

				private:
					CubismRendererProfile_OpenGLES2(QOpenGLContext* context);
					virtual ~CubismRendererProfile_OpenGLES2();
					void Save();
					void Restore();
					void SetGlEnable(GLenum index, GLboolean enabled);
					void SetGlEnableVertexAttribArray(GLuint index, GLint enabled);

					GLint _lastArrayBufferBinding;
					GLint _lastElementArrayBufferBinding;
					GLint _lastProgram;
					GLint _lastActiveTexture;
					GLint _lastTexture0Binding2D;
					GLint _lastTexture1Binding2D;
					GLint _lastVertexAttribArrayEnabled[4];
					GLboolean _lastScissorTest;
					GLboolean _lastBlend;
					GLboolean _lastStencilTest;
					GLboolean _lastDepthTest;
					GLboolean _lastCullFace;
					GLint _lastFrontFace;
					GLboolean _lastColorMask[4];
					GLint _lastBlending[4];
					GLint _lastFBO;
					GLint _lastViewport[4];

					QOpenGLContext* _context;
				};

				class CubismRenderer_OpenGLES2 : public CubismRenderer
				{
					friend class CubismRenderer;
					friend class CubismClippingManager_OpenGLES2;
					friend class CubismShader_OpenGLES2;

				public:
					void Initialize(Framework::CubismModel* model);
					void BindTexture(csmUint32 modelTextureNo, GLuint glTextureNo);
					const csmMap<csmInt32, GLuint>& GetBindedTextures() const;
					void SetClippingMaskBufferSize(csmInt32 size);
					csmInt32 GetClippingMaskBufferSize() const;

				protected:
					CubismRenderer_OpenGLES2(QOpenGLContext* context);
					virtual ~CubismRenderer_OpenGLES2();
					void DoDrawModel();
					void DrawMesh(
						csmInt32 textureNo,
						csmInt32 indexCount,
						csmInt32 vertexCount,
						csmUint16* indexArray,
						csmFloat32* vertexArray,
						csmFloat32* uvArray,
						csmFloat32 opacity,
						CubismBlendMode colorBlendMode,
						csmBool invertedMask);
#ifdef CSM_TARGET_ANDROID_ES2
				public:
					static void SetExtShaderMode(csmBool extMode, csmBool extPAMode = false);
					static void ReloadShader();
#endif // CSM_TARGET_ANDROID_ES2

				private:
					CubismRenderer_OpenGLES2(const CubismRenderer_OpenGLES2&);
					CubismRenderer_OpenGLES2& operator=(const CubismRenderer_OpenGLES2&);
					static void DoStaticRelease();
					void PreDraw();
					void PostDraw() { };
					virtual void SaveProfile();
					virtual void RestoreProfile();
					void SetClippingContextBufferForMask(CubismClippingContext* clip);
					CubismClippingContext* GetClippingContextBufferForMask() const;
					void SetClippingContextBufferForDraw(CubismClippingContext* clip);
					CubismClippingContext* GetClippingContextBufferForDraw() const;

					csmMap<csmInt32, GLuint> _textures;
					csmVector<csmInt32> _sortedDrawableIndexList;
					CubismRendererProfile_OpenGLES2 _rendererProfile;
					CubismClippingManager_OpenGLES2* _clippingManager;
					CubismClippingContext* _clippingContextBufferForMask;
					CubismClippingContext* _clippingContextBufferForDraw;

					CubismOffscreenFrame_OpenGLES2 _offscreenFrameBuffer;

					QOpenGLContext* _context;
				};
} } } }