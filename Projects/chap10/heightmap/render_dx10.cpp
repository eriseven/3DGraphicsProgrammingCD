#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "Gut.h"
#include "GutHeightmap_DX10.h"

#include "render_data.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10RasterizerState *g_pNoCull = NULL;

static CGutHeightmapDX10 g_Heightmap;
static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	// ���Jheightmap
	if ( !g_Heightmap.LoadHeightmapTexture("../../textures/GrandCanyon.tga") )
		return false;

	g_Heightmap.SetRange(Vector4(-50.0f, -50.0f, -5.0f), Vector4(50.0f, 50.0f, 10.0f));

	if ( !g_Heightmap.LoadShader("../../shaders/gmodel_dx10.hlsl") )
		return false;

	if ( !g_Heightmap.BuildMesh(255,255) )
		return false;
	
	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);
		desc.CullMode = D3D10_CULL_NONE;

		g_pDevice->CreateRasterizerState(&desc, &g_pNoCull);
		g_pDevice->RSSetState(g_pNoCull);
	}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.5f, 1000.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	g_Heightmap.Release();
	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.5f, 1000.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// ���o�I�sGutCreateGraphicsDeviceDX10�ɩҲ��ͪ�D3D10����
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	// �M���C��
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// �M��Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	//
	/*
	sModelLight light;

	light.vPosition = g_Lights[0].m_Position;
	light.vDirection = g_Lights[0].m_Direction;
	light.vDiffuse = g_Lights[0].m_Diffuse;
	light.vSpecular = g_Lights[0].m_Specular;
	*/

	//g_Heightmap.Render(g_Control.GetObjectMatrix(), g_Control.GetViewMatrix(), g_proj_matrix, &light);
	g_Heightmap.Render(g_Control.GetObjectMatrix(), g_Control.GetViewMatrix(), g_proj_matrix);
	// ���ݵw�鱽����, �M��~��s�e��
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_