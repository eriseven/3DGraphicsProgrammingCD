#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// �ϥ�zbuffer test
	glEnable(GL_DEPTH_TEST);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// �]�w��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

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
	// `�]�w���ҥ�`
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float *)&g_vGlobal_AmbientLight);
	// `�ϥ�two side lighting�\��`
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	// `�]�w������西�����ϥ���O`
	glMaterialfv(GL_FRONT, GL_AMBIENT, &g_vMaterialAmbient[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &g_vMaterialDiffuse[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &g_vMaterialSpecular[0]);
	glMaterialfv(GL_FRONT, GL_EMISSION, &g_vMaterialEmissive[0]);
	glMaterialf (GL_FRONT, GL_SHININESS, g_fMaterialShininess);
	// `�]�w�������I�����ϥ���O`
	glMaterialfv(GL_BACK, GL_AMBIENT, &g_vMaterialAmbient_Back[0]);
	glMaterialfv(GL_BACK, GL_DIFFUSE, &g_vMaterialDiffuse_Back[0]);
	glMaterialfv(GL_BACK, GL_SPECULAR, &g_vMaterialSpecular_Back[0]);
	glMaterialfv(GL_BACK, GL_EMISSION, &g_vMaterialEmissive_Back[0]);
	glMaterialf (GL_BACK, GL_SHININESS, g_fMaterialShininess_Back);
	// `�}�ҥ����\��`
	glEnable(GL_LIGHTING);
	// `�}�Ҳ�0����`
	glEnable(GL_LIGHT0);
	// `��@��(x,y,z,w)���V�q�ǤJGL_POSITION, �p�Gw�ȬO0�|�Q�����O��V`
	glLightfv(GL_LIGHT0, GL_POSITION, (float *)&(-g_vLightDirection) );
	// `������Ambient��`
	glLightfv(GL_LIGHT0, GL_AMBIENT, &g_vLightAmbient[0]); 
	// `������Diffuse��`
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &g_vLightDiffuse[0]);
	// `������Specular��`
	glLightfv(GL_LIGHT0, GL_SPECULAR, &g_vLightSpecular[0]);
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	// `�⥿�V��ϦV�������e�X��`
	glDisable(GL_CULL_FACE);
	// `�]�w����`
	glLoadMatrixf( (float *) &g_view_matrix);
	SetupLightingOpenGL();
	// `�]�w�n�ΰ}�C���覡�ǤJ���I��m�򭱦V`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VN), &g_Quad[0].m_Position);
	glNormalPointer(GL_FLOAT, sizeof(Vertex_VN), &g_Quad[0].m_Normal);
	// `�p��îM�J�ഫ�x�}`
	Matrix4x4 world_view_matrix = g_world_matrix * g_view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	// `�e�X�@���x��`
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
