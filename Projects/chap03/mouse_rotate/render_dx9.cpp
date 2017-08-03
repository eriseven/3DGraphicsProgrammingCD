#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 1.0f, 100.0f);
	
	// �]�w�����ഫ�x�}
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// �]�w���Y�ഫ�x�}
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ��������
	//device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->Clear(
		0, NULL, // �M����ӵe�� 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // �M���C���Z buffer 
		D3DCOLOR_ARGB(0, 0, 0, 0), // �]�w�n���C��M���¦�
		1.0f, // �]�w�n��Z�ȲM��1, �]�N�O�����Y�̻�
		0 // �]�w�n��Stencil buffer�M��0, �b�o�S�t.
	);
	
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 

	// �]�w��Ʈ榡
	// D3DFVF_XYZ = �ϥ�4�ӯB�I�ƨӰO����m
	// D3DFVF_DIFFUSE = �ϥ�32bits��ƫ��A�ӰO��BGRA�C��
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_object_matrix);

	// �e�X���r��8����u
	device->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST, // ���w�ҭn�e���򥻹ϧκ��� 
		0, // �|�ϥΪ��̤p���I�s��, �ƹ�W�S�Ӥj�γB
		5, // ���I�}�C�̦��X�ӳ��I
		6, // �n�e�X�X�Ӱ򥻹ϧ�
		g_indices, // ���ް}�C
		D3DFMT_INDEX16, // ���ް}�C�����A
		g_vertices, // ���I�}�C
		sizeof(Vertex_VC) // ���I�}�C�̨C�ӳ��I���O���鶡�Z
	); 

	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
