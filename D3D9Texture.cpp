#include "D3D9Texture.h"

D3D9Texture::D3D9Texture(RwRaster* raster) :
    D3D9BaseTexture(raster),
   mD3D9Texture(nullptr)
{
    Initialize();
}

D3D9Texture::~D3D9Texture()
{
    Unitialize();
}

void D3D9Texture::Initialize()
{
    if (mD3D9Texture)
        return;
    
    if (mRaster == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    HRESULT hr;
    if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
    {
        hr = _RwD3DDevice->CreateTexture(mRaster->width, mRaster->height, 1, D3DUSAGE_DEPTHSTENCIL, rasExt->d3dFormat, D3DPOOL_DEFAULT, &mD3D9Texture, nullptr);
    }
    else
    {
        hr = _RwD3DDevice->CreateTexture(mRaster->width, mRaster->height, (mRaster->cFormat & rwRASTERFORMATMIPMAP) ? 0 : 1, 
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0), rasExt->d3dFormat, D3DPOOL_MANAGED, &mD3D9Texture, nullptr);
    }

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9Texture::Initialize - failed to create d3d9 texture");
        throw std::runtime_error("failed to create d3d9 texture");
    }
}

void D3D9Texture::Unitialize()
{
    SAFE_RELEASE(mD3D9Texture);
}

void D3D9Texture::Lock(uint flags, uint level, void* pixelsIn)
{
    if (mRaster == nullptr || mD3D9Texture == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = mD3D9Texture->GetSurfaceLevel(level, &(rasExt->lockedSurface));

    if (SUCCEEDED(hr))
        hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, NULL, flags);
}

void D3D9Texture::Unlock(void* rasterIn)
{
    if (mRaster == nullptr || mD3D9Texture == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    rasExt->lockedSurface->UnlockRect();
    rasExt->lockedSurface->Release();
}