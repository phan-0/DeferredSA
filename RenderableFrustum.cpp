#include "RenderableFrustum.h"

RenderableFrustum::RenderableFrustum()
{
	m_nearPlane = 0.0;
	m_farPlane = 0.0;
	m_viewWindow.x = m_viewWindow.y = 0.0;
	LTM = nullptr;
}

RenderableFrustum::~RenderableFrustum()
{
}

void RenderableFrustum::SetViewMatrix(RwMatrix* LTM)
{
	this->LTM = LTM;
}

void RenderableFrustum::SetViewWindow(float x, float y)
{
	m_viewWindow.x = x;
	m_viewWindow.y = y;
}

void RenderableFrustum::SetClipPlane(float nearPlane, float farPlane)
{
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
}

void RenderableFrustum::InitGraphicsBuffer()
{
	/*m_vertexBuffer = new VertexBuffer();
	m_vertexBuffer->Allocate(13, sizeof(RwIm3DVertex));
	m_indexBuffer = new RwIndexBuffer();
	m_indexBuffer->Allocate(13);*/

}

void RenderableFrustum::RenderFrustum(bool ortho)
{
	 RwImVertexIndex FrustumIndices[] = {
		// Near plane lines
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		// Sides
		0, 4,
		1, 5,
		2, 6,
		3, 7,

		// Far plane lines
		4, 5,
		5, 6,
		6, 7,
		7, 4
	};
	static RwRGBA yellow = {0, 255, 0,  95};
	static RwRGBA red = {255,   0, 0, 255};

	/*
	 * 0:                Camera origin (center of projection)
	 * 1,  2,  3,  4:    View plane
	 * 5,  6,  7,  8:    Near clip-plane
	 * 9,  10, 11, 12:   Far clip-plane
	 */
	RwIm3DVertex frustum[13];

	/* Line index */
	static RwImVertexIndex indicesL[] =
	{
		1,  2,  2,  3,  3,  4,  4,  1,
		5,  6,  6,  7,  7,  8,  8,  5,
		9, 10, 10, 11, 11, 12, 12,  9,
		5,  9,  6, 10,  7, 11,  8, 12,
		0,  0
	};

	///* Triangle index */
	 RwImVertexIndex indicesT[] =
	{
		 5,  6, 10,
		10,  9,  5,
		 6,  7, 11,
		11, 10,  6,
		 7,  8, 12,
		12, 11,  7,
		 8,  5,  9,
		 9, 12,  8,

		 7,  6,  5,
		 5,  8,  7,
		 9, 10, 11,
		11, 12,  9
	};

	static RwReal signs[4][2] =
	{
		{  1,  1 },
		{ -1,  1 },
		{ -1, -1 },
		{  1, -1 }
	};

	RwInt32 i = 0;
	RwInt32 j = 0;
	RwInt32 k = 0;


	Math::AABB box = m_frustum.GetBoundingBox();




	XMFLOAT4X4 m;
	XMStoreFloat4x4(&m, mProj);

	float fov =  2* atan(1.0f / m.m[1][1]) * 180.0f / PI;
	float aspect = m.m[1][1] / m.m[0][0];

	auto C = m.m[2][2];
	auto D = m.m[2][3];
	float nearClip = D / (C - 1.0f);
	float farClip = D / (C + 1.0f);

	// get clip planes
	//float nearClip = -m.m[3][2] / m.m[2][2];
	//float farClip = m.m[3][2] / (1- m.m[2][2]);

	PrintMessage("%f %f %f %f", fov, aspect, C, D);
	RwReal depth[3];
	depth[0] = 1.0f;
	depth[1] = nearClip;
	depth[2] = farClip;
	/* Origin */
	RwIm3DVertexSetPos(&frustum[k], 0, 0, 0.0f);
	k++;

	float    SinFov;
	float    CosFov;
	XMScalarSinCos(&SinFov, &CosFov, 0.5f * XMConvertToRadians(90.0f));

	float Height = CosFov / SinFov;
	float Width = Height / 1.0;

	RwV2d offset = {0, 0};
	RwV2d viewWindow = {fov, fov};
	//PrintMessage("%f %f", viewWindow.x, viewWindow.y);
	/* View Window */
	for(i = 0; i < 3; i++)
	{
		for(j = 1; j < 5; j++)
		{
			if(ortho)
			{
				RwIm3DVertexSetPos(&frustum[k],
								   -offset.x + (signs[j - 1][0] * viewWindow.x)
								   + (depth[i] * offset.x),
								   offset.y + (signs[j - 1][1] * viewWindow.y)
								   - (depth[i] * offset.y),
								   depth[i]);
			}
			
			else
			{
				RwIm3DVertexSetPos(&frustum[k],
								   -offset.x + depth[i] *
								   ((signs[j - 1][0] * viewWindow.x) + offset.x),
								   offset.y + depth[i] *
								   ((signs[j - 1][1] * viewWindow.y) - offset.y),
								   depth[i]);
			}
			k++;
		}
	}
	/*
	 * Set color & alpha for the lines...
	 */
	for(i = 0; i < 5; i++)
	{
		RwIm3DVertexSetRGBA(&frustum[i],
							red.red, red.green, red.blue, red.alpha);
	}

	for(i = 5; i < 13; i++)
	{
		RwIm3DVertexSetRGBA(&frustum[i],
							yellow.red, yellow.green, yellow.blue, 255);
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);

	/*
	 * Draw Lines...
	 */
	if(RwIm3DTransform(frustum, 13, LTM, rwIM3D_ALLOPAQUE))
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indicesL, 34);

		RwIm3DEnd();
	}

	/*
	 * Set color & alpha for the triangles...
	 */
	for(i = 5; i < 13; i++)
	{
		RwIm3DVertexSetRGBA(&frustum[i],
							yellow.red, yellow.green, yellow.blue, yellow.alpha);
	}

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);

	/*
	 * Draw triangles...
	 */
	if(RwIm3DTransform(frustum, 13, LTM, 0))
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, indicesT, 36);

		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);

	//XMFLOAT3* corners = m_frustum.GetCorners();

	//RwIm3DVertex verts[24];
	//verts[0].objVertex = *(RwV3d*)&corners[0];
	//verts[1].objVertex = *(RwV3d*)&corners[1];
	//verts[2].objVertex = *(RwV3d*)&corners[1];
	//verts[3].objVertex = *(RwV3d*)&corners[2];
	//verts[4].objVertex = *(RwV3d*)&corners[2];
	//verts[5].objVertex = *(RwV3d*)&corners[3];
	//verts[6].objVertex = *(RwV3d*)&corners[3];
	//verts[7].objVertex = *(RwV3d*)&corners[0];

	//verts[8].objVertex = *(RwV3d*)&corners[0];
	//verts[9].objVertex = *(RwV3d*)&corners[4];
	//verts[10].objVertex = *(RwV3d*)&corners[1];
	//verts[11].objVertex = *(RwV3d*)&corners[5];
	//verts[12].objVertex = *(RwV3d*)&corners[2];
	//verts[13].objVertex = *(RwV3d*)&corners[6];
	//verts[14].objVertex = *(RwV3d*)&corners[3];
	//verts[15].objVertex = *(RwV3d*)&corners[7];

	//verts[16].objVertex = *(RwV3d*)&corners[4];
	//verts[17].objVertex = *(RwV3d*)&corners[5];
	//verts[18].objVertex = *(RwV3d*)&corners[5];
	//verts[19].objVertex = *(RwV3d*)&corners[6];
	//verts[20].objVertex = *(RwV3d*)&corners[6];
	//verts[21].objVertex = *(RwV3d*)&corners[7];
	//verts[22].objVertex = *(RwV3d*)&corners[7];
	//verts[23].objVertex = *(RwV3d*)&corners[4];
	//RwImVertexIndex indices[] =
	//{
	//	0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
	//	8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15,
	//	16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
	//};

	//
	//for(UINT i = 0; i < 24; i++)
	//{
	//	//RwIm3DVertexSetPos(&verts[i], corners[i].x, corners[i].y, corners[i].z);
	//	RxObjSpace3DLitVertexSetColor(&verts[i], &yellow);
	//}

	////if(RwIm3DTransform(verts, 8, 0, rwIM3D_ALLOPAQUE))
	////{
	////	RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, FrustumIndices, 24);
	////	RwIm3DEnd();
	////}


	//if(RwIm3DTransform(verts, 24, 0, 0 ))
	//{
	//	RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
	//	RwIm3DEnd();
	//}
}