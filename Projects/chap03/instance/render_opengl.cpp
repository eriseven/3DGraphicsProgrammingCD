#include <windows.h>
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `��v�x�}`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, 1.0f, 1.0f, 100.0f);
	// `�]�w�����ഫ�x�}`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// `�S�ư�`
	return true;
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// `�]�w�nGPU�n�h��Ū�����I���`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	// `�]�w�n�ܧ�GL_MODELVIEW�x�}`
	glMatrixMode(GL_MODELVIEW);	

	// `4�Ӫ��r�𪺦�m`
	Vector4 pos[4] = 
	{
		Vector4(-1.0f, -1.0f, 0.0f),
		Vector4( 1.0f, -1.0f, 0.0f),
		Vector4(-1.0f,  1.0f, 0.0f),
		Vector4( 1.0f,  1.0f, 0.0f),
	};

	Matrix4x4 world_matrix;
	Matrix4x4 world_view_matrix;

	for ( int i=0; i<4; i++ )
	{
		// `�o��첾�x�}`
		world_matrix.Translate_Replace(pos[i]); 
		world_view_matrix = world_matrix * g_view_matrix;
		// `�]�w�ഫ�x�}'
		glLoadMatrixf( (float *) &world_view_matrix);
		// `�e�X���r��8����u`
		glDrawArrays(GL_LINES, 0, 16);
	}

	// `��I��backbuffer�e�{�X��`
	GutSwapBuffersOpenGL();
}
