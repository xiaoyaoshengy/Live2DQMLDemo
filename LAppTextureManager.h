#pragma once

#include <string>
#include "CubismSDK/Framework/Type/csmVector.hpp"
#include <QOpenGLFunctions>

class QQuickWindow;

class LAppTextureManager
{
public:
	struct TextureInfo
	{
		GLuint id;
		int width;
		int height;
		std::string fileName;
	};

	LAppTextureManager();
	~LAppTextureManager();
	inline unsigned int Premultiply(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
	{
		return static_cast<unsigned>(\
			(red * (alpha + 1) >> 8) | \
			((green * (alpha + 1) >> 8) << 8) | \
			((blue * (alpha + 1) >> 8) << 16) | \
			(((alpha)) << 24) \
				);
	}
	TextureInfo* CreateTextureFromPngFile(std::string fileName);
	void ReleaseTextures();
	void ReleaseTexture(Csm::csmUint32 textureId);
	void ReleaseTexture(std::string fileName);
	TextureInfo* GetTextureInfoById(GLuint textureId) const;
	void SetWindow(QQuickWindow* win);

private:
	Csm::csmVector<TextureInfo*> _textures;

	QQuickWindow* _window;
};

