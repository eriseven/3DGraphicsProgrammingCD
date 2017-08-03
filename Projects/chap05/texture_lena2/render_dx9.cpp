#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTGA.h"

#include "render_data.h"

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
	// ���Y�y�Шt���ഫ�x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// ���J�K��
	TGAImg tga_loader;
	if( IMG_OK!=tga_loader.Load("../../textures/lena_rgba.tga") )
		return false;
	// ���ͶK�Ϫ���
	if ( D3D_OK!=device->CreateTexture(tga_loader.GetWidth(), tga_loader.GetHeight(), 
		1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &g_pTexture, NULL) )
		return false;
	int image_size = tga_loader.GetWidth() * tga_loader.GetHeight() * 4;
	// ����K�ϸ��
	D3DLOCKED_RECT locked_rect;
	g_pTexture->LockRect(0, &locked_rect, NULL, 0);
	unsigned char *target = (unsigned char *)locked_rect.pBits;
	unsigned char *source = tga_loader.GetImg();
	for ( int y=0; y<tga_loader.GetHeight(); y++ )
	{
		for ( int x=0; x<tga_loader.GetWidth(); x++ )
		{
			// D3DFMT_A8R8G8B8���O����ƦC���ǬOBGRA
			// �n���RGBA�ƦC���O���鰵�ഫ
			target[0] = source[2];
			target[1] = source[1];
			target[2] = source[0];
			target[3] = source[3];

			target+=4;
			source+=4;
		}
	}
	g_pTexture->UnlockRect(0);

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
	// ���Y�y�Шt���ഫ�x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �����e��
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 
	// 
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
	// �]�w��Ʈ榡
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
	// �M�ζK��
	device->SetTexture(0, g_pTexture);
	// �e�X��l
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
