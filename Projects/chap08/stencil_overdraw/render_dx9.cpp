#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

CGutModel_DX9 g_Model_DX9;

static Matrix4x4 g_projection_matrix;

void InitStateDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
}

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	InitStateDX9();

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);

	return true;
}

bool ReleaseResourceDX9(void)
{
	g_Model_DX9.Release();
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
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	InitStateDX9();
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->BeginScene(); 
	// `�����e��`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	// `�e�X����, �ç�s Stencil Buffer.`
	{
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

		// `�Ұ� Stencil Test`
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		// `�]�w Stencil Test ����, �����û�����.`
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		// `���W Stencil Buffer ���e`
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
		// `�e�X����`
		g_Model_DX9.Render();
		// `���A�h�ܧ� Stencil Buffer ���e`
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	}

	Matrix4x4 identity_matrix = Matrix4x4::IdentityMatrix();
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &identity_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &identity_matrix);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &identity_matrix);
	device->SetFVF(D3DFVF_XYZ);
	device->SetRenderState(D3DRS_ZENABLE, FALSE);

	// `�٭����]�w`
	sModelMaterial_DX9 material;
	material.Submit();

	// `�]�w�C��ӷ��� texture factor`
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

	// `�κ��ХܥX�u��s1��������`
	{	
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILREF, 1);
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0,255,0,255));
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_V));
	}
	// `���Ŧ�ХܥX��s2��������`
	{	
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILREF, 2);
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0,0,255,255));
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_V));
	}
	// `�ά���ХܥX��s3��������`
	{	
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILREF, 3);
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255,0,0,255));
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_V));
	}
	// `�Υզ�ХܥX��s�W�L3��������`
	{	
		// `�p�G3�p��stencil buffer����, ����~����.`
		// `�]�N�Ostencil buffer�ȭn�j��3���N��`
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESS);
		device->SetRenderState(D3DRS_STENCILREF, 3);
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255,255,255,255));
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_V));
	}
	// `�٭�]�w`
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	// `�]�w�C��ӷ���` diffuse * texture
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
