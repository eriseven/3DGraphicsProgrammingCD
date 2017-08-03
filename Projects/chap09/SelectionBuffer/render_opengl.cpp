#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_projection_matrix;

static CGutModel_OpenGL g_models[2];

bool InitResourceOpenGL(void)
{
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	g_models[0].ConvertToOpenGLModel(&g_sun_model);
	g_models[1].ConvertToOpenGLModel(&g_earth_model);

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
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

static void RenderSolarSystemOpenGL(bool bSelection)
{
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wv_matrix = world_matrix * view_matrix;
	Matrix4x4 world_view_matrix;

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	// �Ӷ�
	world_view_matrix = g_sun_matrix * wv_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);

	if ( bSelection ) glPushName(1); // �]�w�W�٥N�X��1
	g_models[0].Render();
	if ( bSelection ) glPopName();

	// �a�y
	world_view_matrix = g_earth_matrix * wv_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);

	if ( bSelection ) glPushName(2); // �]�w�W�٥N�X��2
	g_models[1].Render();
	if ( bSelection ) glPopName();
}

#define BUFSIZE 1024
GLuint selectBuf[BUFSIZE];

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `���o�����j�p`
	int w, h;
	GutGetWindowSize(w, h);
	// `�M���e��`
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `�b�ù��W�e�X��P�B��`
	RenderSolarSystemOpenGL(false);

	/* 
		`�Ұ�Selection Buffer, �w��|�յۥh���e�@��3D����,` 
		`�����|��s�e��, �u�|�O���U���b�e�����Y�ӽd�򤺩Ҧ�3D����.`
	*/
	int x,y,buttons[3];
	GutGetMouseState(x, y, buttons);
	if ( buttons[0] )
	{
		GLint viewport[4];

		glSelectBuffer(BUFSIZE,selectBuf);
		glRenderMode(GL_SELECT);
		glInitNames();

		glGetIntegerv(GL_VIEWPORT,viewport);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		// `�]�wSelection Buffer���ˬd�d��`
		gluPickMatrix(x, viewport[3]-y, 3, 3, viewport);
		float fRatio = (viewport[2]+0.0) / viewport[3];
		glMultMatrixf( (float *)&g_projection_matrix );
		// `�bSelection Buffer���e�X3D����`
		RenderSolarSystemOpenGL(true);
		int hits = glRenderMode(GL_RENDER);
		
		GLuint *ptr = selectBuf;
		GLuint Zmin = 0xffffffff;
		GLuint SelectedObject = 0;
		// `��X�ˬd�d��, �Z�����Y�̪񪺪���.`
		for ( int i=0; i<hits; i++, ptr+=4 )
		{
			if ( ptr[1] < Zmin )
			{
				Zmin = ptr[1];
				SelectedObject = ptr[3];
			}
		}

		switch(SelectedObject)
		{
		default:
		case 0:
			printf("Nothing\r");
			break;
		case 1:
			printf("Sun    \r");
			break;
		case 2:
			printf("Earth  \r");
			break;
		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
