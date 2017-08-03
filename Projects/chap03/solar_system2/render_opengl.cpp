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
	// ��v�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_CULL_FACE);

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
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// `�]�w�n�ΰ}�C���覡�ǤJ���I��m���C��`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 world_view_matrix;

	glMatrixMode(GL_MODELVIEW);
	// `���J���Y�ഫ�x�}`
	glLoadMatrixf( (float *) &view_matrix );
	glPushMatrix(); // `��ثe���x�}�s��stack��`
		// `�Ӷ�, �M�J�ƹ������౱��.`
		glRotatef(FastMath::RadianToDegree(g_fRotate_X), 1.0f, 0.0f, 0.0f);
		glRotatef(FastMath::RadianToDegree(g_fRotate_Y), 0.0f, 1.0f, 0.0f);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pSunVertices);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pSunVertices[0].m_RGBA);
		glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
		// `���P`
		glPushMatrix(); // `��ثe���x�}�s��stack��`
			float mercury_rotate_y = 360.0f * (g_simulation_days / days_a_year_mercury); 
			glRotatef(mercury_rotate_y, 0.0f, 1.0f, 0.0f);
			glTranslatef(mercury_to_sun_distance, 0.0f, 0.0f);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
			glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
		// `�qstack��pop�X�@�ӯx�}, �îM�Ψ�ثe�ҫ��w�n�ާ@���x�}`GL_MODELVIEW
		glPopMatrix(); 
		// `���P`
		glPushMatrix(); // `��ثe���x�}�s��stack��`
			float venus_rotate_y = 360.0f * (g_simulation_days / days_a_year_venus); 
			glRotatef(venus_rotate_y, 0.0f, 1.0f, 0.0f);
			glTranslatef(venus_to_sun_distance, 0.0f, 0.0f);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
			glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
		// `�qstack��pop�X�@�ӯx�}, �îM�Ψ�ثe�ҫ��w�n�ާ@���x�}`GL_MODELVIEW
		glPopMatrix(); 
		// `�a�y`
		glPushMatrix();// `��ثe���x�}�s��stack��`
			float earth_rotate_y = 360.0f * (g_simulation_days / days_a_year); 
			glRotatef(earth_rotate_y, 0.0f, 1.0f, 0.0f);
			glTranslatef(earth_to_sun_distance, 0.0f, 0.0f);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pEarthVertices);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pEarthVertices[0].m_RGBA);
			glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
			// `��y`
			float moon_rotate_y = 360.0f * (g_simulation_days / days_a_month);
			glRotatef(moon_rotate_y, 0.0f, 1.0f, 0.0f);
			glTranslatef(moon_to_earth_distance, 0.0f, 0.0f);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
			glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
			// `�qstack��pop�X�@�ӯx�}, �îM�Ψ�ثe�ҫ��w�n�ާ@���x�}GL_MODELVIEW`
		glPopMatrix(); 
	glPopMatrix();

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
