#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

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
	glEnable(GL_NORMALIZE);
	glDisable(GL_CULL_FACE);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	g_Model_OpenGL.Release();

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

static void RenderModelOpenGL(bool mirror, Vector4 *pPlane)
{
	Matrix4x4 view_matrix;

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vLookAt = g_Control.m_vLookAt;
		Vector4 vUp = g_Control.m_vUp;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorVector(vUp, *pPlane);

		Matrix4x4 temp_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);

		// �]���O��g, �b�ഫ�����Y�y�Шt��n���ӥ��k��ժ��ʧ@.
		Matrix4x4 mirror_x;
		mirror_x.Identity();
		mirror_x.Scale(-1.0f, 1.0f, 1.0f);

		view_matrix = temp_matrix * mirror_x;
		g_mirror_view_matrix = view_matrix;

		// �k����ի�, 3���Ϊ����I�ƦC���Ƿ|�Q�ϹL��.
		glFrontFace(GL_CW);
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
		glFrontFace(GL_CCW);
	}

	glMatrixMode(GL_MODELVIEW);
	Matrix4x4 world_view_matrix = g_Control.GetObjectMatrix() * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);

	g_Model_OpenGL.Render();

	glFrontFace(GL_CCW);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	// `�M���e��`
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// `�e�X����`
	RenderModelOpenGL(false, NULL);
	// `�]�w�ഫ�x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();;
	Matrix4x4 world_view_matrix = view_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	sModelMaterial_OpenGL material;
	material.Submit(NULL);
	// `�]�w���I��Ʈ榡`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V), &g_Quad_v[0].m_Position);

	// `�e�X��l, �P�ɧ���l������stencil buffer�]��1.`
	{
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `����l������zbuffer�M��1.0`
	{
		glStencilFunc(GL_EQUAL, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// `��z�Ȫ���X�d��]�w��1~1, �]�N�Oz�û���X1.`
		glDepthRange(1.0f, 1.0f);
		glDepthFunc(GL_ALWAYS);
		// `�u��szbuffer,���ݭn��s�C��.`
		glColorMask(false, false, false, false);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// `��_��s�C�⪺�\��`
		glColorMask(true, true, true, true);
		glDepthFunc(GL_LESS);
		// `��z�Ȫ��d���٭쬰0~1`
		glDepthRange(0.0f, 1.0f);
	}
	// `�e�X��l�̪�����`
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		RenderModelOpenGL(true, &vPlane);
		glDisable(GL_STENCIL_TEST);
	}
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
