#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXSHADER9 g_pVertexShaderDX9 = NULL;
static LPDIRECT3DPIXELSHADER9  g_pPixelShaderDX9 = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ���JVertex Shader
	g_pVertexShaderDX9 = GutLoadVertexShaderDX9_HLSL("../../shaders/vertex_color.shader", "VS", "vs_1_1");
	if ( g_pVertexShaderDX9==NULL )
		return false;
	// ���JPixel Shader
	g_pPixelShaderDX9 = GutLoadPixelShaderDX9_HLSL("../../shaders/vertex_color.shader", "PS", "ps_2_0");
	if ( g_pPixelShaderDX9==NULL )
		return false;
	// �p���v�ഫ�x�}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pVertexShaderDX9);
	SAFE_RELEASE(g_pPixelShaderDX9);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	GutResetGraphicsDeviceDX9();

	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

// `�ϥ�Direct3D9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�����e��`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 
	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 view_proj_matrix = view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix;
	// `�]�w��Ʈ榡`
	// `D3DFVF_XYZ = �ϥ�3�ӯB�I�ƨӰO����m`
	// `D3DFVF_DIFFUSE = �ϥ�32bits��ƫ��A�ӰO��BGRA�C��`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 
	// `�]�wShader`
	device->SetVertexShader(g_pVertexShaderDX9);
	device->SetPixelShader(g_pPixelShaderDX9);
	// `�Ӷ�`
	world_view_proj_matrix = g_sun_matrix * view_proj_matrix;
	device->SetVertexShaderConstantF(0, (const float *)&world_view_proj_matrix, 4);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
	// `�a�y`
	world_view_proj_matrix = g_earth_matrix * view_proj_matrix;
	device->SetVertexShaderConstantF(0, (const float *)&world_view_proj_matrix, 4);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
	// `��G`
	world_view_proj_matrix = g_moon_matrix * view_proj_matrix;
	device->SetVertexShaderConstantF(0, (const float *)&world_view_proj_matrix, 4);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pMoonVertices, sizeof(Vertex_VC) );
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
