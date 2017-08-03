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
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// �]�w�����ഫ�x�}
	g_projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���J�K��
	g_pTexture0 = GutLoadTexture_DX9("../../textures/brickwall_broken.tga");
	g_pTexture1 = GutLoadTexture_DX9("../../textures/spotlight_effect.tga");

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
	GutResetGraphicsDeviceDX9();
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;
	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	g_projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	//device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
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

// `�ϥ�Direct3D9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 
	// `�]�w��Ʈ榡`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 

	{
		Matrix4x4 IdentityMatrix; 
		IdentityMatrix.Identity();
		
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &IdentityMatrix);
		device->SetTexture(0, g_pTexture0);

		if ( g_bStencil )
		{
			// `�Ұ�` Stencil Buffer Test
			device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
			// `�� Stencil �Ѧҭȳ]�w 1`
			device->SetRenderState(D3DRS_STENCILREF, 1);
			// `Stencil Test �P�_����]�w���û�����`
			device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
			// `������ Stencil �Ѧҭȶ�J Stencil Buffer ��`
			device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		}
		else
		{
			// ����Stencil Buffer Test
			device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		}

		// `�]�w Alpha Test ����`
		device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		device->SetRenderState(D3DRS_ALPHAREF, 128);

		// `�e�X���, �|�P�ɧ�sFramebuffer�W���C���Stencil Buffer.`
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	}

	{
		if ( g_bStencil )
		{
			// `�U�� 3 ��{���X�|�]�w���u��s�e���W Stencil �Ȭ� 1 ������`
			// `�� Stencil �Ѧҭȳ]�w 1`
			device->SetRenderState(D3DRS_STENCILREF, 1);
			// `Stencil Test �P�_����]�w������ Stencil �ѦҭȤ~����`
			device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
			// `Stencil Test �P�_���߮ɡA�O�d Stencil Buffer �ȡA���h��s���C`
			device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		}

		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		// `���Y�y�Шt�ഫ�x�}`
		Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
		device->SetTexture(0, g_pTexture1);
		// `�}�ҲV��\��`
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		// `�V�⤽��` = source_color * 1 + dest_color * 1
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		// `�e�X���W`
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
		// `����Alpha Test�\��`
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
	device->Present( NULL, NULL, NULL, NULL );
}
