#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXSHADER9 g_pVertexShaderDX9 = NULL;
static LPDIRECT3DPIXELSHADER9  g_pPixelShaderDX9 = NULL;
static LPDIRECT3DVERTEXDECLARATION9 g_pVertexShaderDecl = NULL;

bool InitResourceDX9(void)
{
	// `���JVertex Shader`
	g_pVertexShaderDX9 = GutLoadVertexShaderDX9_HLSL(
		"../../shaders/vertex_transform.shader", "VS", "vs_1_1");
	if ( g_pVertexShaderDX9==NULL )
		return false;
	// `���JPixel Shader`
	g_pPixelShaderDX9 = GutLoadPixelShaderDX9_HLSL(
		"../../shaders/vertex_transform.shader", "PS", "ps_2_0");
	if ( g_pPixelShaderDX9==NULL )
		return false;

	// `���Y�y�Шt�ഫ�x�}`
	Matrix4x4 view_matrix = 
		GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `��v�ഫ�x�}`
	Matrix4x4 projection_matrix = 
		GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 1.0f, 100.0f);
	// `��e��ӯx�}�ۭ�`
	Matrix4x4 view_projection_matrix = 
		view_matrix * projection_matrix;
	
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�]�w�����ഫ�x�}`
	device->SetVertexShaderConstantF(0, (float *)&view_projection_matrix, 4);

	return true;
}

bool ReleaseResourceDX9(void)
{
	if ( g_pVertexShaderDX9 )
	{
		g_pVertexShaderDX9->Release();
		g_pVertexShaderDX9 = NULL;
	}

	if ( g_pPixelShaderDX9 )
	{
		g_pPixelShaderDX9->Release();
		g_pPixelShaderDX9 = NULL;
	}

	return true;
}

// `�ϥ�DirectX9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
    device->Clear(
		0, NULL, // `�M����ӵe��`
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // `�M���C���Z Buffer`
		D3DCOLOR_ARGB(0, 0, 0, 0), // `�]�w�n���C��M���¦�`
		1.0f, // `�]�w�n��Z�ȲM��1, �]�N�O�����Y�̻�.`
		0 // `�]�w�n��Stencil buffer�M��0, �b�o�S�t.`
	);
	
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 
	// `�]�w��Ʈ榡`
	device->SetFVF(D3DFVF_XYZW); 
	// `�]�wVertex/Pixel Shader`
	device->SetVertexShader(g_pVertexShaderDX9);
	device->SetPixelShader(g_pPixelShaderDX9);
	// `�e�X���r��8����u`
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, g_vertices, sizeof(Vector4)); 
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
