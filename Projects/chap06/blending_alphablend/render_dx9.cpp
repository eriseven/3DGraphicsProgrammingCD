#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"

#include "render_data.h"

static LPDIRECT3DTEXTURE9 g_pTexture0 = NULL;
static LPDIRECT3DTEXTURE9 g_pTexture1 = NULL;

static Matrix4x4 g_projection_matrix;

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// ���J�K��
	g_pTexture0 = GutLoadTexture_DX9("../../textures/brickwall.tga");
	g_pTexture1 = GutLoadTexture_DX9("../../textures/lena.tga");

	// trilinear
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// trilinear
	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture0);
	SAFE_RELEASE(g_pTexture1);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// trilinear
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// trilinear
	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 
	// �]�w��Ʈ榡
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1); 

	{
		Matrix4x4 IdentityMatrix; 
		IdentityMatrix.Identity();
		
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &IdentityMatrix);
		device->SetTexture(0, g_pTexture0);
		// ZBuffer���ձ���]���û�����
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		// �e�X�x��, �P�ɷ|�M��ZBuffer
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_FullScreenQuad, sizeof(Vertex_VCT));
		// ZBuffer���ձ���]���p��
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	}

	{
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		// �]�w�ഫ�x�}
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		//
		device->SetTexture(0, g_pTexture1);
		// �}�ҲV��\��
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		// source_blend_factor = source_color.a
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		// dest_blend_factor = (1 - source_color.a)
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		// �V�⤽�� = source_color.rgba * (source_color.a) + dest_color.rgba * (1 - source_color.a)
		// direct3D���w�|������X�K��alpha��, �⥦�令���Ialpha�Ȭۭ�.
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		// �e�X�x��
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VCT));
		// ����Alpha Test�\��
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
