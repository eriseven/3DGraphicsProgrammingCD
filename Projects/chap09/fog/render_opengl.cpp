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
static Vector4 g_vFogColor(0.5f, 0.5f, 0.5f, 1.0f);

void InitState(float aspect)
{
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
}

bool InitResourceOpenGL(void)
{
	InitState(1.0f);

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
	InitState(aspect);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// �M���e��
	glClearColor(g_vFogColor[0], g_vFogColor[1], g_vFogColor[2], g_vFogColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = object_matrix * view_matrix;
	// �]�w��v�ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	// �]�w���Y�ഫ�x�}
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	// �]�w��
	glEnable(GL_FOG); // �}�������\��
	glFogfv(GL_FOG_COLOR, &g_vFogColor[0]); // �C��
	
	switch(g_iFogMode)
	{
	case 0:
		glDisable(GL_FOG);
		break;
	case 1:
		// ���H�Z���u���ܿ@
		glFogi(GL_FOG_MODE, GL_LINEAR);
		// �]�w�����d��
		glFogf(GL_FOG_START, 0.0f);
		glFogf(GL_FOG_END, 10.0f);
		// �p�⤽����
		// (fog_end - distance_to_camera) / (fog_end - fog_start)
		break;
	case 2:
		// `�M�Ϋ��ƨ禡���ܤƪ��� `
		glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, 0.5f);
		// �p�⤽����
		// power(e, -(fog_density * distance_to_camera))
		break;
	case 3:
		// `�M�Ϋ��ƨ禡���ܤƪ��� `
		glFogi(GL_FOG_MODE, GL_EXP2);
		glFogf(GL_FOG_DENSITY, 0.5f);
		// �p�⤽����
		// power(e, -(fog_density * distance_to_camera)^2)
		break;
	}

	// �e�X�ҫ�
	g_Model_OpenGL.Render();
	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
