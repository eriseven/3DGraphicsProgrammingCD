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
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

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
	// �]�w�nGPU�n�h��Ū�����I���
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	// �]�w�n�ܧ�GL_MODELVIEW�x�}
	glMatrixMode(GL_MODELVIEW);	

	for ( int i=0; i<4; i++ )
	{
		// `�إ��ഫ�x�}`
		Matrix4x4 world_matrix;
		world_matrix.Scale_Replace(g_scale[i]); // `�إ��Y��x�}`
		world_matrix[3] = g_position[i]; // `������첾��J�x�}���U��.`

		// `�]�w�ഫ�x�}`
		Matrix4x4 world_view_matrix = world_matrix * g_view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);
		
		// �e�X���r��8����u
		glDrawElements(
			GL_LINES, // ���w�ҭn�e���򥻹ϧκ���
			16, // ���X�ӯ��ޭ�
			GL_UNSIGNED_SHORT, // ���ޭȪ����A
			g_indices // ���ޭȰ}�C
		);
	}

	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
