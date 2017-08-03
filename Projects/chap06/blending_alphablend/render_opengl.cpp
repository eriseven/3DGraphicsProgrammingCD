#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

GLuint g_Texture0_ID = 0;
GLuint g_Texture1_ID = 0;

bool InitResourceOpenGL(void)
{
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	// ���J�K��
	g_Texture0_ID = GutLoadTexture_OpenGL("../../textures/brickwall.tga");
	// �M��Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// ���J�K��
	g_Texture1_ID = GutLoadTexture_OpenGL("../../textures/lena.tga");
	// �M��Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);

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
	if ( g_Texture0_ID )
	{
		// ����K�ϸ귽
		glDeleteTextures(1, &g_Texture0_ID);
		g_Texture0_ID = 0;
	}

	if ( g_Texture1_ID )
	{
		// ����K�ϸ귽
		glDeleteTextures(1, &g_Texture1_ID);
		g_Texture1_ID = 0;
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
	// �]�w�n�ΰ}�C���覡�ǤJ���I��m���C��
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	{
		// ���A�γ�@�C��ӲM���e��,
		// ���ӵe���οj��Ϥ��\�L�h

		// ��modelview, projection�x�}�]�����x�}
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); // ��projection�x�}�s��bstack��
		glLoadIdentity();

		// ���ݭnzbuffer test
		glDepthFunc(GL_ALWAYS);
		// �M�ζK��
		glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
		// �e�X�x��, �P�ɷ|�M��ZBuffer
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VCT), &g_FullScreenQuad[0].m_Position);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VCT), &g_FullScreenQuad[0].m_Color);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VCT), &g_FullScreenQuad[0].m_Texcoord);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// ��s��bstack����projection���^��
		glPopMatrix();
		glDepthFunc(GL_LESS);
	}

	{
		// �}�ҲV��\��
		glEnable(GL_BLEND);
		// source_blend_factor = 1
		// dest_blend_factor = 1
		// �V�⤽��= source_color * 1 + dest_color * 1
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// �M�ζK��
		glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
		// �M�J�ഫ�x�}
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
		Matrix4x4 world_view_matrix = world_matrix * view_matrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		// �e�X�x��
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VCT), &g_Quad[0].m_Position);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VCT), &g_Quad[0].m_Color);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VCT), &g_Quad[0].m_Texcoord);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// �����V��\��
		glDisable(GL_BLEND);
	}

	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
