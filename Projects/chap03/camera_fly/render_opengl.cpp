#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// ��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_CULL_FACE);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// �S�ư�
	return true;
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// �M���e��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// �]�w�n�ΰ}�C���覡�ǤJ���I��m���C��
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);

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

			// �]�w�ഫ�x�}
			Matrix4x4 object_matrix; object_matrix.Identity();
			
			if ( c==0 )
			{
				object_matrix = view_matrix;
				object_matrix.Translate(grid_position);

				glLoadMatrixf( (float *) &object_matrix);
				// �]�wGPU�n�h��Ū�����I�y�и��
				glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), &g_road_vertices[0].m_Position);
				// �]�wGPU�n�h��Ū�����I�C����
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), &g_road_vertices[0].m_RGBA);
				// �e�X�a�O
				glDrawElements(
					GL_TRIANGLE_STRIP, // ���w�ҭn�e���򥻹ϧκ���
					4, // ���X�ӯ��ޭ�
					GL_UNSIGNED_SHORT, // ���ޭȪ����A
					g_road_trianglestrip_indices // ���ޭȰ}�C
				);
			}
			else
			{
				// �]�w���r�𪺰���
				object_matrix.Scale_Replace(1.0f, (float) c, 1.0f);
				object_matrix[3] = grid_position;
				Matrix4x4 world_view_matrix = object_matrix * view_matrix;
				glLoadMatrixf( (float *) &world_view_matrix);
				// �]�wGPU�n�h��Ū�����I�y�и��
				glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), &g_pyramid_vertices[0].m_Position);
				// �]�wGPU�n�h��Ū�����I�C����
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), &g_pyramid_vertices[0].m_RGBA);
				// �e�X���r��
				glDrawElements(
					GL_TRIANGLE_FAN, // ���w�ҭn�e���򥻹ϧκ���
					6, // ���X�ӯ��ޭ�
					GL_UNSIGNED_SHORT, // ���ޭȪ����A
					g_pyramid_trianglefan_indices // ���ޭȰ}�C
				);
			}

			grid_position[2] += 1.0f;
		}

		grid_position[0] += 1.0f;
	}

	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
