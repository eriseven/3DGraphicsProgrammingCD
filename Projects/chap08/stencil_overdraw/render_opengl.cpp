#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;

static CGutModel_OpenGL g_Model_OpenGL;

bool InitResourceOpenGL(void)
{
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	return true;
}

// callback function. �����j�p���ܮɷ|�Q�I�s, �öǤJ�s�������j�p.
void ResizeWindowOpenGL(int width, int height)
{
	// �ϥηs�������j�p�����s��ø�ϸѪR��
	glViewport(0, 0, width, height);
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;

	// `�e�X����, �ç�s Stencil Buffer.`
	{
		// `�e�X����`
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);

		// `�Ұ� Stencil Test`
		glEnable(GL_STENCIL_TEST);
		// `�]�w Stencil Test ����, �����û�����.`
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		// `���W Stencil Buffer ���e`
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		// `�e�X����`
		g_Model_OpenGL.Render();
		// `���A�h�ܧ� Stencil Buffer ���e`
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	sModelMaterial_OpenGL material;
	material.Submit(NULL);

	// `�����\����ӵe��, ���ݭn���í�����.`
	glDepthFunc(GL_ALWAYS);

	// `�]�w���I��Ʈ榡`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V), &g_Quad[0].m_Position);

	// `�κ��Хܥu��s�L1��������`
	{
		glStencilFunc(GL_EQUAL, 1, 0xff);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `���Ŧ�Хܧ�s�L2��������`
	{
		glStencilFunc(GL_EQUAL, 2, 0xff);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `�ά���ӼХܧ�s�W�L3���H�W������`
	{
		glStencilFunc(GL_EQUAL, 3, 0xff);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `�ά���ӼХܧ�s�W�L3���H�W������`
	{
		// `�p�G3�p��stencil buffer����, ����~����. `
		// `�]�N�Ostencil buffer�ȭn�j��3���N��`
		glStencilFunc(GL_LESS, 3, 0xff); 
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `�٭�]�w`
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDepthFunc(GL_LESS);
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
