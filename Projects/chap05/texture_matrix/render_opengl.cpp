#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

GLuint g_Texture_ID = 0;

bool InitResourceOpenGL(void)
{
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	// ���J�K��
	g_Texture_ID = GutLoadTexture_OpenGL("../../textures/lena_rgb.tga");
	// �M��Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// ��W�L0-1�d�򪺮y�и�Ū��k�]��REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// �⥿�V��ϦV�������e�X��
	glDisable(GL_CULL_FACE);
	// �Ұ�2D�K�ϥ\��
	glEnable(GL_TEXTURE_2D);
	// �Ұ�zbuffer���í�����
	glEnable(GL_DEPTH_TEST);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_Texture_ID )
	{
		// ����K�ϸ귽
		glDeleteTextures(1, &g_Texture_ID);
		g_Texture_ID = 0;
	}

	return true;
}

// callback function. �����j�p���ܮɷ|�Q�I�s, �öǤJ�s�������j�p.
void ResizeWindowOpenGL(int width, int height)
{
	// �ϥηs�������j�p�����s��ø�ϸѪR��
	glViewport(0, 0, width, height);
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// �M���e��
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// �M�ζK��
	glBindTexture(GL_TEXTURE_2D, g_Texture_ID);
	// �]�w�ഫ�x�}
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// �M�J�K�Ϯy���ഫ�x�}
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf( (float *)&g_texture_matrix );
	// �e�X�x��
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
