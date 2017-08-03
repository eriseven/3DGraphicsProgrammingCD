#include <d3dx9.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"

static LPDIRECT3DTEXTURE9 g_pTexture = NULL;

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// ���J�K��
	g_pTexture = GutLoadTexture_DX9("../../textures/checkerboard.tga");

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);

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
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

// �ϥ�Direct3D9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�����e��`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 
	// `�]�w�x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &object_matrix);
	// `�]�w��Ʈ榡`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
	// `�M�ζK��`
	device->SetTexture(0, g_pTexture);

	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	
	switch(g_iFilterMode)
	{
	case 1: // linear filtering
		// `�K�ϳQ��j��, ���̱���4���I�Ӥ���.`
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		// `�K�ϳQ�Y�p��, ���̱���4���I�ӥ���.`
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		// `���ϥ�Mipmap`
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		break;
	case 2: // bilinear filtering
		// `�K�ϳQ��j��, ���̱���4���I�Ӥ���.`
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		// `�K�ϳQ�Y�p��, ���̱���4���I�ӥ���.`
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		// `���̱���j�p���ϼh`
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		break;
	case 3: // trilinear filtering
		// `�K�ϳQ��j��, ���̱���4���I�Ӥ���.`
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		// `�K�ϳQ�Y�p��, ���̱���4���I�ӥ���.`
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		// `���̱���j�p����ӹϼh, �M��A����.`
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		break;
	case 4: // anisotropic filtering
		// `�|�ھ��Y�񪺪��e��ҨӨ��ζK��, ���T�w�O�����X�ӹ�.`
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
		device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
		// `���̱���j�p����ӹϼh, �M��A����.`
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		break;
	}
	// `�e�X�x��`
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
