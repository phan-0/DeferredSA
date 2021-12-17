#pragma once
#include "CommonD.h"
class RenderTarget
{
public:
	RenderTarget(D3DFORMAT format);
	~RenderTarget();

	void Initialize();
	void Release();
	void CopyFromSurface(LPSURFACE surface);

	RwRaster* GetRaster();
	LPSURFACE GetSurface();
private:
	D3DFORMAT mFormat;
	RwRaster* mRaster;
	LPSURFACE mSurface;
};

