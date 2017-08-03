#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"
#include "GutModel_OpenGL.h"

#include "render_data.h"

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// �]�w��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// ���J�K��
	const char *texture_array[] = 
	{
		"../../textures/uffizi_right.tga",
		"../../textures/uffizi_left.tga",
		"../../textures/uffizi_top.tga",
		"../../textures/uffizi_bottom.tga",
		"../../textures/uffizi_back.tga", // `�k��y�Шt�W Z+ �����Y���.`
		"../../textures/uffizi_front.tga" // `�k��y�Шt�W Z- �����Y�e��.`
	};

	g_TextureID = GutLoadCubemapTexture_OpenGL(texture_array);

	// �ϥ�CUBEMAP�K�ϥ\��
	glEnable(GL_TEXTURE_CUBE_MAP);
	// �M��CUBEMAP�K��
	glBindTexture( GL_TEXTURE_CUBE_MAP, g_TextureID );

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
	// `�ϥ�CUBEMAP�K�ϥ\��`
	glEnable(GL_TEXTURE_CUBE_MAP);
	// `�M��CUBEMAP�K��`
	glBindTexture( GL_TEXTURE_CUBE_MAP, g_TextureID );
	// Trilinear filter
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// `�]�w�ഫ�x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// `�]�wVertex Array`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VN), g_pSphereVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex_VN), g_pSphereVertices[0].m_Normal);
	// `�e�y`
	glDrawElements(
		GL_TRIANGLES, // `���w�ҭn�e���򥻹ϧκ���`
		g_iNumSphereIndices, // `���X�ӯ��ޭ�`
		GL_UNSIGNED_SHORT, // `���ޭȪ����A`
		g_pSphereIndices // `���ޭȰ}�C`
	);
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
