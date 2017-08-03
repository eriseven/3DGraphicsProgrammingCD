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
	//
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
}

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 
	InitStateDX9();
	//
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
	// ���oDirect3D9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	InitStateDX9();
}

static void RenderModelDX9(bool mirror, Vector4 *pPlane)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 view_matrix;

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vLookAt = g_Control.m_vLookAt;
		Vector4 vUp = g_Control.m_vUp;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorVector(vUp, *pPlane);

		Matrix4x4 temp_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);

		// �]���O��g, �b�ഫ�����Y�y�Шt��n���ӥ��k��ժ��ʧ@.
		Matrix4x4 mirror_x;
		mirror_x.Identity();
		mirror_x.Scale(-1.0f, 1.0f, 1.0f);

		view_matrix = temp_matrix * mirror_x;

		// �k����ի�, 3���Ϊ����I�ƦC���Ƿ|�Q�ϹL��.
		GutSetDX9BackFace(D3DCULL_CCW);
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		GutSetDX9BackFace(D3DCULL_CW);
	}

	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	g_Model_DX9.Render();

	GutSetDX9BackFace(D3DCULL_CW);
}

// �ϥ�Direct3D9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	device->BeginScene(); 
	// `�����e��`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 150, 255), 1.0f, 0);
	// `�e�X����`
	RenderModelDX9(false, NULL);

	// `�e�X�譱, �P�ɧ�stencil�ȳ]��1.`
	{	
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_STENCILREF, 1);

		Matrix4x4 identity_matrix = Matrix4x4::IdentityMatrix();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &identity_matrix);
		
		sModelMaterial_DX9 material;
		material.m_Material.Diffuse.r = material.m_Material.Diffuse.g = material.m_Material.Diffuse.b = material.m_Material.Diffuse.a = 0.0f;
		material.Submit();
		// `�e�X�譱`
		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad_v, sizeof(Vertex_V));
	}
	// `���譱�d��ZBuffer�M��1.0`
	{
		// `�]�wstencil test����, �u��s�譱�d�򪺹���.`
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS); // `����z test`
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0); // `����RGBA����X`
		// `�g�ѭק�viewport Z�d�򪺤�k, ��3D����Z�ȿ�X�T�w��1.`
		int w, h; 
		GutGetWindowSize(w, h);
		D3DVIEWPORT9 viewport;
		viewport.X = viewport.Y = 0;
		viewport.Width = w; viewport.Height = h;
		viewport.MinZ = 1.0f; viewport.MaxZ = 1.0f;
		device->SetViewport(&viewport);
		// `�e�X�譱`
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad_v, sizeof(Vertex_V));
		// `��Z�Ƚd���٭즨0-1`
		viewport.MinZ = 0.0f; viewport.MaxZ = 1.0f;
		device->SetViewport(&viewport);
		// `�٭�zbuffer test�����ձ���`
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		// `���s���}RGBA����X`
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 
			D3DCOLORWRITEENABLE_RED | 
			D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE |
			D3DCOLORWRITEENABLE_ALPHA);
	}
	// `�b�譱�d��̵e�X��g������`
	{
		RenderModelDX9(true, &vPlane);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
