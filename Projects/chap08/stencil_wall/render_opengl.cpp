#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;

GLuint g_Texture0_ID = 0;
GLuint g_Texture1_ID = 0;

bool InitResourceOpenGL(void)
{
	// ���Y�y�Шt�ഫ�x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// ��v�x�}
	Matrix4x4 projection_matrix = GutMatrixOrthoRH_OpenGL(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// ���J�K��
	g_Texture0_ID = GutLoadTexture_OpenGL("../../textures/brickwall_broken.tga");
	// �M��Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// ���J�K��
	g_Texture1_ID = GutLoadTexture_OpenGL("../../textures/spotlight_effect.tga");
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
	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;
	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	Matrix4x4 projection_matrix = GutMatrixOrthoRH_OpenGL(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// `�]�w�n�ΰ}�C���覡�ǤJ���I��m���C��`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// `���ݭn` zbuffer test
	glDisable(GL_DEPTH_TEST);
	
	{
		// `��modelview, projection�x�}�]�����x�}.`
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); // `��projection�x�}�s��bstack��`
		glLoadIdentity();
		
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);

		if ( g_bStencil )
		{
			// `�Ұ�` Stencil Buffer Test
			glEnable(GL_STENCIL_TEST);
			// `Stencil Test �P�_����]�w���û�����`
			// `Stencil �Ѧҭȳ]�w�� 1`
			// `Stencil Mask �]�w�� 0xff = 255`
			glStencilFunc(GL_ALWAYS, 1, 0xff);
			// `Stencil Test ���߮�, �� Stencil �Ѧҭȶ�J Stencil Buffer ��.`
			// `�e��� GL_KEEP �O���� Stencil �� ZBuffer Test �����߮�, ���h���� Stencil Buffer`
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		// `�M�ζK��`
		glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
		// `�e�X�x��, �P�ɷ|�M��ZBuffer.`
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// `��s��bstack����projection���^��`
		glPopMatrix();
		glDepthFunc(GL_LESS);
		glDisable(GL_ALPHA_TEST);
	}

	{
		if ( g_bStencil )
		{
			// `�u��s�e���WStencil�Ȭ�1������`
			glStencilFunc(GL_EQUAL, 1, 0xff);
			// `���h��s Stencil Buffer �ƭ�`
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}

		// `�}�ҲV��\��`
		glEnable(GL_BLEND);
		// `�V�⤽��` = source_color * 1 + dest_color * 1
		glBlendFunc(GL_ONE, GL_ONE);
		// `�M�ζK��`
		glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
		// `�M�J�ഫ�x�}`
		Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		// `�e�X�x��`
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// `�����V��\��`		
		glDisable(GL_BLEND);
	}

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
