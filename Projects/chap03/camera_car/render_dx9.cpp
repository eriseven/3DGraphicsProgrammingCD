#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	int num_vertices = sizeof(g_pyramid_vertices)/sizeof(Vertex_VC);

	// ��RGBA���C�����ഫ��DX9 BGRA�榡
	for ( int i=0; i<num_vertices; i++ )
	{
		unsigned char temp = g_pyramid_vertices[i].m_RGBA[0];
		g_pyramid_vertices[i].m_RGBA[0] = g_pyramid_vertices[i].m_RGBA[2];
		g_pyramid_vertices[i].m_RGBA[2] = temp;
	}

	num_vertices = sizeof(g_road_vertices)/sizeof(Vertex_VC);
	for ( int i=0; i<num_vertices; i++ )
	{
		unsigned char temp = g_road_vertices[i].m_RGBA[0];
		g_road_vertices[i].m_RGBA[0] = g_road_vertices[i].m_RGBA[2];
		g_road_vertices[i].m_RGBA[2] = temp;
	}

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

	// `�M���e��`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 

	// `�]�w��Ʈ榡`
	// `D3DFVF_XYZ = �ϥ�3�ӯB�I�ƨӰO����m`
	// `D3DFVF_DIFFUSE = �ϥ�32bits��ƫ��A�ӰO��BGRA�C��`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	Vector4 border(-15.0f, 0.0f, -15.0f);
	Vector4 grid_position = border;

	const int grids_x = 30;
	const int grids_z = 30;

	for ( int x=0; x<grids_x; x++ )
	{
		int grid_x = x & 0x07;
		grid_position[2] = border[2];

		for ( int z=0; z<grids_z; z++ )
		{
			int grid_z = z & 0x07;
			char c = g_map[grid_x][grid_z];

			// `�]�w�ഫ�x�}`
			Matrix4x4 object_matrix;
			object_matrix.Translate_Replace(grid_position);

			if ( c==0 ) // `����`
			{
				device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &object_matrix);
				// `�e�X�a�O`
				device->DrawIndexedPrimitiveUP(
					D3DPT_TRIANGLESTRIP,
					0,
					4,
					2,
					g_road_trianglestrip_indices,
					D3DFMT_INDEX16,
					g_road_vertices,
					sizeof(Vertex_VC)
				);
			}
			else // `���r��`
			{
				// `�]�w���r�𪺰���`
				object_matrix.Scale_Replace(1.0f, (float) c, 1.0f);
				object_matrix[3] = grid_position;
				device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &object_matrix);
				// `�e�X���r��`
				device->DrawIndexedPrimitiveUP(
					D3DPT_TRIANGLEFAN, // `���w�ҭn�e���򥻹ϧκ���`
					0, // `�|�ϥΪ��̤p���I�s��, �ƹ�W�S�Ӥj�γB.`
					5, // `���I�}�C�̦��X�ӳ��I`
					4, // `�n�e�X�X�Ӱ򥻹ϧ�`
					g_pyramid_trianglefan_indices,	// `���ް}�C`
					D3DFMT_INDEX16,		// `���ް}�C�����A`
					g_pyramid_vertices,	// `���I�}�C`
					sizeof(Vertex_VC)	// `���I�}�C�̨C�ӳ��I���O���鶡�Z`
				);
			}

			grid_position[2] += 1.0f;
		}

		grid_position[0] += 1.0f;
	}

	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
