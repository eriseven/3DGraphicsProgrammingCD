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
	//glShadeModel(GL_FLAT);

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

	// �]�w�nGPU�n�h��Ū�����I�y�и��
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), &g_vertices[0].m_Position);
	// �]�w�nGPU�n�h��Ū�����I�C����
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), &g_vertices[0].m_RGBA);
	// �]�w�n�ܧ�GL_MODELVIEW�x�}
	glMatrixMode(GL_MODELVIEW);	

	// �]�w�ഫ�x�}
	Matrix4x4 world_view_matrix = g_object_matrix * g_view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	// �e�X���r��8����u
	glDrawElements(
		GL_TRIANGLES, // ���w�ҭn�e���򥻹ϧκ���
		18, // ���X�ӯ��ޭ�
		GL_UNSIGNED_SHORT, // ���ޭȪ����A
		g_indices // ���ޭȰ}�C
	);

	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
