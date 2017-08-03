#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�p��X���Y�y�Шt���ഫ�x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 1.0f, 100.0f);
	// `�]�w�����ഫ�x�}`
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// `�]�w���Y�ഫ�x�}`
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// `��������`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

// `�ϥ�Direct3D9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 
	// `�]�w��Ʈ榡`
	device->SetFVF(D3DFVF_XYZ); 

	// `���ਤ��`
	static float angle = 0.0f;
	angle += 0.01f;

	// `�]�w����x�}`
	Matrix4x4 world_matrix;
	world_matrix.RotateZ_Replace(angle);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	// `�e�X���r��8����u`
	device->DrawIndexedPrimitiveUP(
		D3DPT_LINELIST, // `���w�ҭn�e���򥻹ϧκ��� `
		0, // `�|�ϥΪ��̤p���I�s��, �ƹ�W�S�Ӥj�γB.`
		5, // `���I�}�C�̦��X�ӳ��I`
		8, // `�n�e�X�X�Ӱ򥻹ϧ�`
		g_indices,		// `���ް}�C`
		D3DFMT_INDEX16, // `���ް}�C�����A`
		g_vertices,		// `���I�}�C`
		sizeof(Vector4)	// `���I�}�C�̨C�ӳ��I���O���鶡�Z`
	);

	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	
	// `��I��backbuffer�e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
