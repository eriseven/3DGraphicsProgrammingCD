#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"

static Matrix4x4 g_projection_matrix;

static CGutModel_OpenGL g_Model_OpenGL;
static CGutModel_OpenGL g_SpotLightModel_OpenGL;

static sModelMaterial_OpenGL g_material_stencilpass;
static sModelMaterial_OpenGL g_material_spotlightpass;

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
	g_SpotLightModel_OpenGL.ConvertToOpenGLModel(&g_SpotLightModel);

	g_material_stencilpass.m_bCullFace = false;
	
	g_material_spotlightpass.m_bBlend = true;
	g_material_spotlightpass.m_SrcBlend = GL_ONE;
	g_material_spotlightpass.m_DestBlend = GL_ONE;
	g_material_spotlightpass.m_Diffuse.Set(0.3f);

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
	// `�ഫ�x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	// `�e����`
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		g_Model_OpenGL.Render();
	}

	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &view_matrix);
	
	// stencil pass
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilFunc(GL_ALWAYS, 1, 0xff);

		// �e�X�������Y�����A�P�ɻ��W Stencil Buffer�C
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		// `�I�s Render �ɶǤJ 0 �N���ݭn�M�Υ������]�w`
		g_SpotLightModel_OpenGL.Render(0);

		// `�e�X�I�����Y�����A�P�ɻ��� Stencil Buffer�C`
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		// `�I�s Render �ɶǤJ 0 �N���ݭn�M�Υ������]�w`
		g_SpotLightModel_OpenGL.Render(0);

		// `�٭�]�w`
		glCullFace(GL_BACK);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	// `�e�X���W�Ӯg�쪺�ϰ�`
	{
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 0xff);

		// �� g_material_spotlightpass �Ө��N�ҫ���������]�w
		CGutModel_OpenGL::SetMaterialOverwrite(&g_material_spotlightpass);
		g_SpotLightModel_OpenGL.Render();
		// `�٭�A���ҫ��ϥΥ��쥻������]�w�C`
		CGutModel_OpenGL::SetMaterialOverwrite(NULL);
	}
	glDisable(GL_STENCIL_TEST);
	// `�Υ��`����k�e�X���W`
	g_SpotLightModel_OpenGL.Render();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
