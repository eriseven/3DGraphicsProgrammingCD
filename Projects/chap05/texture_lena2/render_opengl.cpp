#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutTGA.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;
GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// �]�w��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// ���J�K��
	TGAImg tga_loader;
	if( IMG_OK!=tga_loader.Load("../../textures/lena_rgba.tga") )
		return false;
	// ���ͤ@�ӶK�Ϫ���
	glGenTextures( 1, &g_TextureID );
	// �ϥ�g_TextureID�K�Ϫ���
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// RGBA�Ҧ�
	glTexImage2D( GL_TEXTURE_2D, 0, 3, 
		tga_loader.GetWidth(), tga_loader.GetHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, tga_loader.GetImg() );
	// `�]�w��ܶK�ϳQ�Y�p�ɨϥνu�ʤ���`
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// `�]�w��ܶK�ϳQ��j�ɨϥνu�ʥ~��`
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// �ϥ�2D�K�ϥ\��
	glEnable(GL_TEXTURE_2D);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_TextureID )
	{
		glDeleteTextures(1, &g_TextureID);
		g_TextureID = 0;
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
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// �M���e��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// �⥿�V��ϦV�������e�X��
	glDisable(GL_CULL_FACE);
	// �]�w�n�ΰ}�C���覡�ǤJ���I��m���C��
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// �e�X��l
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
