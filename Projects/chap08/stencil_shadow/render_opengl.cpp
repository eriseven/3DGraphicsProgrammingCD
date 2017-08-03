#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;

static CGutModel_OpenGL g_Model_textured_OpenGL;

bool InitResourceOpenGL(void)
{
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_textured_OpenGL.ConvertToOpenGLModel(&g_Model_textured);

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

	Vector4 light_pos(5.0f, 0.0f, 5.0f);
	Vector4 light_lookat(0.0f, 0.0f, 0.0f);
	Vector4 light_up(0.0f, 1.0f, 0.0f);

	Matrix4x4 light_view = GutMatrixLookAtRH(light_pos, light_lookat, light_up);
	Matrix4x4 light_world_view = world_matrix * light_view;
	Matrix4x4 ident_matrix; ident_matrix.Identity();
	Matrix4x4 shadow_matrix;

	// `�إ�`shadow volume
	if ( g_bDirectionalLight )
	{
		g_ShadowVolume.BuildShadowVolume(light_world_view, ident_matrix, 20.0f);
		shadow_matrix = light_view;
		shadow_matrix.FastInvert();
	}
	else
	{
		Matrix4x4 light_proj = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
		g_ShadowVolume.BuildShadowVolume(light_world_view, light_proj, 20.0f);
		shadow_matrix = light_view;
		shadow_matrix.FastInvert();
	}

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	// `�e�X�ҫ�`
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		// glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		g_Model_textured_OpenGL.Render();
	}
	// `�e�X���`
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);

		sModelMaterial_OpenGL material;
		material.m_Diffuse.Set(0.0f, 0.0f, 1.0f);
		material.m_bCullFace = false;
		material.Submit(NULL);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3T2), g_Quad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `����zbuffer��s�\��`
	glDepthMask(GL_FALSE);
	// `�bStencil Buffer�W�ХܥX���v�ϰ�`
	{
		sModelMaterial_OpenGL material;
		material.Submit(NULL);

		world_view_matrix = shadow_matrix * view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);
		// `�]�w���I��Ʈ榡`
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vector4), g_ShadowVolume.m_pShadowVolume);
		// `����szbuffer��framebuffer`
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		// `�u��sStenil Buffer`
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0x01, 0xff);
		// `�e�X�I�����Y����, �P�ɻ���Stencil Buffer.`
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR_WRAP);
		glDrawArrays(GL_TRIANGLES, 0, g_ShadowVolume.m_iNumFaces * 3);
		// `�e�X�������Y����, �P�ɻ��WStencil Buffer.`
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR_WRAP);
		glDrawArrays(GL_TRIANGLES, 0, g_ShadowVolume.m_iNumFaces * 3);
		// `��_��sframebuffer��zbuffer`
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	// `�e�X���v`
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// `�ϥζ¦�A�e�@�����`
		sModelMaterial_OpenGL material;
		material.m_bCullFace = false;
		material.m_Diffuse.Set(0.0f, 0.0f, 0.0f, 1.0f);
		material.Submit(NULL);
		// `�u��sstencil buffer�W�Ȭ��D 0 ������`
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3T2), g_FullScreenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisable(GL_STENCIL_TEST);
	}
	// `�[��shadow volume, ������.`
	if ( g_bDrawShadowVolume )
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);
		glMatrixMode(GL_MODELVIEW);
		world_view_matrix = shadow_matrix * view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);

		// `�ϥζ¦�A�e�@�����`
		sModelMaterial_OpenGL material;
		material.m_bCullFace = false;
		material.m_Diffuse.Set(1.0f);
		material.Submit(NULL);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vector4), g_ShadowVolume.m_pShadowVolume);
		glDrawArrays(GL_TRIANGLES, 0, g_ShadowVolume.m_iNumFaces * 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	// `���s�Ұ�zbuffer��s�\��`
	glDepthMask(GL_TRUE);
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
