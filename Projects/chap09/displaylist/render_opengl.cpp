#include <windows.h>
#include "glew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;
static GLuint g_quad_list[4];

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	const char *extension = (const char *)glGetString(GL_EXTENSIONS);
	printf("OpenGL Extension : \n%s\n", extension);

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// �]�w��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// ���J�K��
	g_TextureID = GutLoadTexture_OpenGL("../../textures/lena_rgba.tga");
	// �ϥ�g_TextureID�K�Ϫ���
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// �]�w��ܶK�Ϯɨϥνu�ʤ���
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// �]�w��ܶK�Ϯɨϥνu�ʥ~��
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// �ϥ�2D�K�ϥ\��
	glEnable(GL_TEXTURE_2D);

	GLint modes[] = {
		GL_REPEAT,// ���U��
		GL_MIRRORED_REPEAT, // ���W��
		GL_CLAMP_TO_EDGE, // �k�U��
		GL_CLAMP_TO_BORDER, // �k�W��
	};

	float border_color[4] = {0.5f, 0.5f, 0.5f, 0.5f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	Vertex_VT *vertex = NULL;

	for ( int i=0; i<4; i++ )
	{
		g_quad_list[i] = glGenLists(1);
		// `�إߤ@���s��Display List�O��`
		glNewList(g_quad_list[i], GL_COMPILE);
			// `�M�ζK��`
			glBindTexture( GL_TEXTURE_2D, g_TextureID );
			// `�ϥζK�Ϥ����\��`
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// `�]�w�K�Ϯy�и�Ū�Ҧ�`
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modes[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modes[i]);
			// `�e�X�ݪO`
			glBegin(GL_QUADS);
				vertex = &g_Quads[i][0];
				glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
				glVertex3fv(vertex->m_Position);

				vertex = &g_Quads[i][1];
				glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
				glVertex3fv(vertex->m_Position);

				vertex = &g_Quads[i][2];
				glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
				glVertex3fv(vertex->m_Position);

				vertex = &g_Quads[i][3];
				glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
				glVertex3fv(vertex->m_Position);
			glEnd();

		glEndList();
	}

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
	// `���Y�y���ഫ�x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `�ഫ�x�}`
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	for ( int i=0; i<4; i++ )
	{
		// `����Display List���ҰO�����ʧ@`
		glCallList(g_quad_list[i]);
	}

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
