#pragma once
#include "CommonD.h"
#include "VolumetricClouds.h"
#include "PostProcessing.h"
enum RenderingStage
{
	stageDeferred,
	stageForward,
	stageDualParaboloidMap,
	stageSphereMap,
	stageReflectionCubemap,
	stageReflection,
	stageRefraction,
	stageCascadeShadow
}extern gRenderState;

class DeferredRendering
{
public:

	DeferredRendering();
	~DeferredRendering();

	PostProcessing* mPostProcessing;
	RwRaster* m_graphicsLight;
	RwRaster* m_graphicsBuffer[4];
	RwRaster* m_shadowScreenRaster;
	RwRaster* m_screenRaster;
	RwRaster* m_volumetricLight;
	VolumetricClouds* mVolumetricClouds;
	void BindFirstPass();
	void BindLastPass();
	void RenderPostProcessing();
	void Initialize();

	void imguiParameters();
private:
	void* PS_DirectLight;
	void* PS_PointAndSpotLight;
	void* PS_TargetLight;
	void* PS_ShadowScreen;
	void* PS_AtmosphereScattering;
	void* PS_VolumetricLight;
	void* PS_VolumetricLightCombine;

	void* VS_Quad;

	void DirectLight();
	void PointAndSpotLight();
	void FinalPass();
	void CascadedShadow();
	void AtmosphericScattering();
	void VolumetricLight();
	void FXAA();
	void Bloom();

};

extern DeferredRendering *DeferredContext;