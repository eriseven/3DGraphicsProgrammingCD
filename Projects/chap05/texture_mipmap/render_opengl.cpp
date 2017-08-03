#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// �]�w��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// ���J�K��
	g_TextureID = GutLoadTexture_OpenGL("../../textures/checkerboard.tga");
	// �ϥ�g_TextureID�K�Ϫ���
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// ��W�L0-1�d�򪺮y�и�Ū��k�]��REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// `�⥿�V��ϦV�������e�X��`
	glDisable(GL_CULL_FACE);
	// `�]�w�n�ΰ}�C���覡�ǤJ���I��m���C��`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
	// `�K�ϳQ��j��, ���̱���4���I�Ӥ���.`
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// `�U���|�]�w�K�ϳQ�Y�p�ɭ԰��k`
	switch(g_iFilterMode)
	{
	case 1: // None
		// `���ϥ�mipmap, ���̪�4���I�ӥ���.`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// `����anisotrophic filter`
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 2: // Bilinear
		// `GL_LINEAR = �K���Y�p�ɨ�4���I�ӥ���`
		// `MIPMAP_NEAREST = �ϥγ̱���j�p���ϼh`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		// `����anisotrophic filter`
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 3: // Trilinear
		// `GL_LINEAR = �K���Y�p�ɨ�4���I�ӥ���`
		// `MIPMAP_LINEAR = �ϥγ̱���j�p����i�ϼh, �M��A����.`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		// `����anisotrophic filter`
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 4: // Anisotropic filter
		// MIPMAP_LINEAR = `�ϥγ̱���j�p����i�ϼh , �M��A���� .`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		// `�Ұ� anisotrophic filter`
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);
		break;
	}

	// `�p��x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = object_matrix * view_matrix;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// `�e�X��l`
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
