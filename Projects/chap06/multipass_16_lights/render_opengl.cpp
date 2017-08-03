#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"
#include "GutModel_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

CGutModel_OpenGL g_Model_OpenGL;

bool InitResourceOpenGL(void)
{
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 1000.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

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

void SetupLightingOpenGL(Light *pLightArray, int num_lights)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &g_vAmbientLight[0]);

	int i;

	for ( i=0; i<num_lights; i++ )
	{
		int LightID = GL_LIGHT0 + i;

		Light *pLight = pLightArray + i;

		glEnable(LightID);
		
		glLightfv(LightID, GL_POSITION, &pLight->m_Position[0]);
		glLightfv(LightID, GL_DIFFUSE, &pLight->m_Diffuse[0]);
		glLightfv(LightID, GL_SPECULAR, &pLight->m_Specular[0]);

		glLightf(LightID, GL_CONSTANT_ATTENUATION,	0.0f);
		glLightf(LightID, GL_LINEAR_ATTENUATION,	2.0f);
		glLightf(LightID, GL_QUADRATIC_ATTENUATION, 0.0f);
	}

	for ( ; i<8; i++ )
	{
		glDisable(GL_LIGHT0 + i);
	}
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `�����í����ճW�h�]���p��ε���` <=
	glDepthFunc(GL_LEQUAL);

	// `�]�w�ഫ�x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	// `�e8����`
	if ( g_iSwitch & 0x01 )
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);
		SetupLightingOpenGL(&g_Lights[0], 8);

		glLoadMatrixf( (float *) &world_view_matrix);
		g_Model_OpenGL.Render();
	}

	// `��8����`
	if ( g_iSwitch & 0x02 )
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);
		SetupLightingOpenGL(&g_Lights[8], 8);

		glLoadMatrixf( (float *) &world_view_matrix);

		if ( g_iSwitch & 0x01 )
		{
			// �p�G�e8�������}, �n�ϥβV��ӧ�U8�����|�[�W�h.
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			// ~SUBMIT_BLEND �|�����M���ɮפ����V��]�w.
			g_Model_OpenGL.Render(~SUBMIT_BLEND);
		}
		else
		{
			g_Model_OpenGL.Render();
		}
	}

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
