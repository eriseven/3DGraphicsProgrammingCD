#include "render_data.h"

#include "Gut.h"
#include "GutDX9.h"
#include "GutWin32.h"

LPDIRECT3DVERTEXDECLARATION9 g_pVertexDecl = NULL;
LPDIRECT3DVERTEXSHADER9 g_pVertexShader = NULL;
LPDIRECT3DPIXELSHADER9 g_pPixelShader = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 500.0f);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//
	g_pVertexShader = GutLoadVertexShaderDX9_HLSL("../../shaders/VertexCompression.hlsl", "VS", "vs_2_0");
	g_pPixelShader = GutLoadPixelShaderDX9_HLSL("../../shaders/VertexCompression.hlsl", "PS", "ps_2_0");

	D3DVERTEXELEMENT9 elems[] =
	{
		{0, 0, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 4, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		D3DDECL_END()
	};

	if ( D3D_OK!=device->CreateVertexDeclaration(elems, &g_pVertexDecl) )
		return false;

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pVertexDecl);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pVertexDecl);
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 500.0f);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	// �M���e��
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 100, 100, 255), 1.0f, 0);
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 
	// view matrix
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wvp_matrix = object_matrix * view_matrix * g_proj_matrix;
	//
	device->SetVertexShaderConstantF(0, (float *)&wvp_matrix, 4);
	device->SetVertexShaderConstantF(4, (float *)&object_matrix, 4);
	device->SetVertexDeclaration(g_pVertexDecl);
	device->SetVertexShader(g_pVertexShader);
	device->SetPixelShader(g_pPixelShader);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pCompressedVertices, sizeof(CompressedVertex) );
	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
