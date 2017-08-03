#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// �]�w��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// �⥿�V��ϦV�������e�X��
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// �S�ư�
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

void SetupLightingOpenGL(void)
{
	// `�}�ҥ����\��`
	glEnable(GL_LIGHTING);
	// `�]�w���ҥ�`
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float *)&g_vGlobal_AmbientLight);
	// `�]�w������誺�ϥ���O`
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float *) &g_vMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float *) &g_vMaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, (float *) &g_vMaterialSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, (float *) &g_vMaterialEmissive);
	// `�]�w����`
	for ( int i=0; i<g_iNumLights; i++ )
	{
		// `�C�ӥ������O��GL_LIGHT0,GL_LIGHT1,GL_LIGHT2�Ӫ��`
		// `���̦b�w�q�ɬO�̶��ǱƦC,�i�H��GL_LIGHT0+n�Ө��o����ID`
		int LightID = GL_LIGHT0 + i;

		if ( g_Lights[i].m_bEnabled )
		{
			// `���}�o����`
			glEnable(LightID);
			
			switch(g_Lights[i].m_eType)
			{
			case LIGHT_DIRECTIONAL:
				// `������V`
				// `��@��(x,y,z,w)���V�q�ǤJGL_POSITION, �p�Gw�ȬO0�|�Q�����O��V`
				// `��V���w�q�� Direct3D �ۤ�, �ҥH�n�[�ӭt��.`
				glLightfv(LightID, GL_POSITION, (float *)&(-g_Lights[i].m_vDirection));
				break;
			case LIGHT_POINT:
				// `������m`
				glLightfv(LightID, GL_POSITION, (float *)&g_Lights[i].m_vPosition);
				// `�H�Z�����I��� `
				// 1/(CONSTANT + LINEAR*d + QUADRATIC*d^2) `��������CONSTANT,LINEAR,QUADRATIC��`
				glLightf(LightID, GL_CONSTANT_ATTENUATION,	g_Lights[i].m_vAttenuation[0]);
				glLightf(LightID, GL_LINEAR_ATTENUATION,	g_Lights[i].m_vAttenuation[1]);
				glLightf(LightID, GL_QUADRATIC_ATTENUATION, g_Lights[i].m_vAttenuation[2]);
				break;
			case LIGHT_SPOT:
				// `������m`
				glLightfv(LightID, GL_POSITION, (float *)&g_Lights[i].m_vPosition);
				// `�H�Z�����I���`
				// 1/(CONSTANT + LINEAR*d + QUADRATIC*d^2) `��������CONSTANT, LINEAR, QUADRATIC��`
				glLightf(LightID, GL_CONSTANT_ATTENUATION,	g_Lights[i].m_vAttenuation[0]);
				glLightf(LightID, GL_LINEAR_ATTENUATION,	g_Lights[i].m_vAttenuation[1]);
				glLightf(LightID, GL_QUADRATIC_ATTENUATION, g_Lights[i].m_vAttenuation[2]);
				// `���W����V`
				glLightfv(LightID, GL_SPOT_DIRECTION, (float *)&g_Lights[i].m_vDirection);
				// `���W���@���@�b����`
				glLightf(LightID, GL_SPOT_CUTOFF, g_Lights[i].m_fSpotlightCutoff * 0.5f);
				// `���W���@�����װI��`
				glLightf(LightID, GL_SPOT_EXPONENT, g_Lights[i].m_fSpotlightExponent);
				break;
			}
			// `������Ambient��`
			glLightfv(LightID, GL_AMBIENT, (float *)&g_Lights[i].m_vAmbientColor); 
			// `������Diffuse��`
			glLightfv(LightID, GL_DIFFUSE, (float *)&g_Lights[i].m_vDiffuseColor);
		}
		else
		{
			// `�����o����`
			glDisable(LightID);
		}
	}
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// �M���e��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	// �]�w�n�ΰ}�C���覡�ǤJ���I��m���C��
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3N3), g_pGridVertices[0].m_Position);
	glNormalPointer (GL_FLOAT, sizeof(Vertex_V3N3), g_pGridVertices[0].m_Normal);
	
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// �M�����Y�ഫ�x�}
	glLoadMatrixf( (float *) &view_matrix);
	// �]�w����, �Ҧ������|�ഫ�����Y�y�Шt,�ҥH�n���M�����Y�ഫ�x�}
	SetupLightingOpenGL();
	
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	// �e�X��l
	glDrawElements (GL_TRIANGLE_STRIP, g_iNumGridIndices, GL_UNSIGNED_SHORT, g_pGridIndices);
	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
