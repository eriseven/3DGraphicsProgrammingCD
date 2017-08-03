#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// ��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// ���J�K��
	g_TextureID = GutLoadTexture_OpenGL("../../textures/brick.tga");
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// �ϥ�2D�K�ϥ\��
	glEnable(GL_TEXTURE_2D);
	// ��W�L0-1�d�򪺮y�и�Ū��k�]��REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// �}�Ҧ۰ʲ��ͶK�Ϯy�Х\��
	// S/T���O�N��K�Ϯy�Ъ�X/Y�b
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	// �]�w���ѦҪ����m�Ӳ��ͶK�Ϯy��
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	// �C�}�ƭȤ��O�N��a*x+b*y+c*z+d*w��a,b,c,d��
	// 1*x+0*y+0*z+0*w = x, �K�Ϯy��x�b�ȵ����I��mx��
	float fvTexcoordScale_S[] = {1.0f, 0.0f, 0.0f, 0.0f};
	glTexGenfv(GL_S, GL_EYE_PLANE, fvTexcoordScale_S);
	// 0*x+1*y+0*z+0*w = y, �K�Ϯy��y�b�ȵ����I��my��
	float fvTexcoordScale_T[] = {0.0f, 1.0f, 0.0f, 0.0f};
	glTexGenfv(GL_T, GL_EYE_PLANE, fvTexcoordScale_T);

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
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &g_Quad[0].m_Position);
	// Trilinear filter
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// �]�w�ഫ�x�}
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// �e�X��l
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
