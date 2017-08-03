#include <windows.h>
// OpenGL 1.2+ & extensions
#include "glew.h"
#include "wglew.h"
// OpenGL 1.2-
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static GLuint g_VertexBufferID = 0;
static GLuint g_IndexBufferID = 0;

bool InitResourceOpenGL(void)
{
	// `��v�x�}`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// `�]�w�����ഫ�x�}`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	
	glMatrixMode(GL_MODELVIEW);	

	// `���ͤ@���s��Vertex Buffer Object`
	glGenBuffers(1, &g_VertexBufferID);
	// `�ϥ�g_VertexBufferID�ҥN��Vertex Buffer Object`
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferID);
	// `�t�mVertex Buffer���j�p, �æP�ɧ�g_pSphereVertices�����e����L�i�h.`
	glBufferData(GL_ARRAY_BUFFER, g_iNumSphereVertices * sizeof(Vertex_VC), g_pSphereVertices, GL_STATIC_COPY);

	// `���ͤ@���s��Index Buffer Object`
	glGenBuffers(1, &g_IndexBufferID);
	// `�ϥ�g_IndexBufferID�ҥN��Index Buffer Object`
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IndexBufferID);
	// `�t�mIndex Buffer���j�p, �æP�ɧ�g_pSphereIndices�����e����L�h.`
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_iNumSphereIndices * 2, g_pSphereIndices, GL_STATIC_COPY);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_VertexBufferID )
	{
		glDeleteBuffers(1, &g_VertexBufferID);
		g_VertexBufferID = 0;
	}

	if ( g_IndexBufferID )
	{
		glDeleteBuffers(1, &g_IndexBufferID);
		g_IndexBufferID = 0;
	}

	return true;
}

void ResizeWindowOpenGL(int width, int height)
{
	glViewport(0, 0, width, height);
	// `��v�x�}`
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// `�]�w�����ഫ�x�}`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// `�ϥ�g\_VertexBufferID�ҥN��Vertex Buffer Object`
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferID);
	// `�ϥ�g\_IndexBufferID�ҥN��Index Buffer Object`
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IndexBufferID);

	// `�]�w�n�ΰ}�C���覡�ǤJ���I��m���C��`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// `VertexPointer/ColorPointer���A�ǤJ���Ц�m`
	// �ӬO�ǤJ�O����۹諸�첾��
	int voffset = (int) &g_pSphereVertices[0].m_Position - (int) &g_pSphereVertices[0];
	int coffset = (int) &g_pSphereVertices[0].m_RGBA - (int) &g_pSphereVertices[0];
	// `�̫�@�ӭȬO�b�O���餤���_�l�첾`
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), (GLvoid *)voffset); // voffset=0
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), (GLvoid *)coffset); // coffset=12

	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &view_matrix);

	glDrawElements(
		GL_TRIANGLES, // `���w�ҭn�e���򥻹ϧκ���`
		g_iNumSphereIndices, // `���X�ӯ��ޭ�`
		GL_UNSIGNED_SHORT, // `���ޭȪ����A`
		0 // `���A�ǤJ�}�C������, �ӬO�ǥί��ޭȰ_�l���첾.`
	);

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
