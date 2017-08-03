#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

static CGutModel_DX9 g_Model_DX9;
static CGutModel_DX9 g_SpotLightModel_DX9;

static sModelMaterial_DX9 g_material_stencilpass;
static sModelMaterial_DX9 g_material_spotlightpass;

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
	g_SpotLightModel_DX9.ConvertToDX9Model(&g_SpotLightModel);

	g_material_stencilpass.m_bCullFace = false;
	
	g_material_spotlightpass.m_bBlend = true;
	g_material_spotlightpass.m_SrcBlend = D3DBLEND_ONE;
	g_material_spotlightpass.m_DestBlend = D3DBLEND_ONE;

	g_material_spotlightpass.m_Material.Diffuse.r = 
	g_material_spotlightpass.m_Material.Diffuse.g = 
	g_material_spotlightpass.m_Material.Diffuse.b = 
	g_material_spotlightpass.m_Material.Diffuse.a = 0.3f;

	return true;
}

bool ReleaseResourceDX9(void)
{
	g_Model_DX9.Release();
	g_SpotLightModel_DX9.Release();

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

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	device->BeginScene(); 
	// �����e��
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
	// �ഫ�x�}
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// �e�X�Ŷ���������
	{
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		g_Model_DX9.Render();
	}
	// �bStencil Buffer���ХܥX���W�Ӯg���ϰ�
	device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	{
		Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &ident_matrix);
		// `���� face culling`
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		// `�Ұ� Two sided stencil test �\��`
		device->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE);
		device->SetRenderState(D3DRS_STENCILREF, 1);
		// `���ɰw��V, �]�N�O�I���� Stencil Test �]�w`
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
		// `�f�ɰw��V, �]�N�O������ Stencil Test �]�w`
		device->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_INCR);
		// `�����C��� ZBuffer ����s`
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		g_SpotLightModel_DX9.Render(0);

		// `�٭�]�w`
		device->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 
			D3DCOLORWRITEENABLE_RED |
			D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE |
			D3DCOLORWRITEENABLE_ALPHA);
	}
	// �e�X���W�Ӯg�쪺�ϰ�
	{
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILREF, 1);

		CGutModel_DX9::SetMaterialOverwrite(&g_material_spotlightpass);
		g_SpotLightModel_DX9.Render();
		CGutModel_DX9::SetMaterialOverwrite(NULL);
	}
	device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	// �e�X���W
	{
		g_SpotLightModel_DX9.Render();
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
