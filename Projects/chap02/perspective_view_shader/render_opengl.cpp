#include <windows.h>
// OpenGL extensions header
#include "glew.h"
#include "wglew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static GLuint g_VertexProgramID = 0;
static GLuint g_FragmentProgramID = 0;

bool InitResourceOpenGL(void)
{
	g_VertexProgramID = GutLoadVertexProgramOpenGL_ASM
		("../../shaders/vertex_transform.glvp");
	if ( g_VertexProgramID==0 )
		return false;

	g_FragmentProgramID = GutLoadFragmentProgramOpenGL_ASM
		("../../shaders/vertex_transform.glpp");
	if ( g_FragmentProgramID==0 )
		return false;

	// `���Y�y�Шt�ഫ�x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `��v�ഫ�x�}`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
	// `�e��ӯx�}�ۭ����G`
	Matrix4x4 view_projection_matrix = view_matrix * projection_matrix;
	// `�]�w�����ഫ�x�}`
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, (float *) &view_projection_matrix);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_VertexProgramID )
	{
		GutReleaseVertexProgramOpenGL(g_VertexProgramID);
		g_VertexProgramID = 0;
	}

	if ( g_FragmentProgramID )
	{
		GutReleaseVertexProgramOpenGL(g_FragmentProgramID);
		g_FragmentProgramID = 0;
	}

	return true;
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `���w�n�ϥ�Shader�i�{���Ƭy�{`
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	// `�]�w�ϥέ���Vertex/Fragment Program`
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, g_VertexProgramID);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, g_FragmentProgramID);

	// `�e�X���r��8����u`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	glDrawArrays(GL_LINES, 0, 16);

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
