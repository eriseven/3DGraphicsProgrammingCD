#include <windows.h>
// OpenGL extensions header
#include "glew.h"
#include "wglew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_view_proj_matrix;
static Matrix4x4 g_proj_matrix;

static GLuint g_DirLight_VP = 0;
static GLuint g_PointLight_VP = 0;
static GLuint g_SpotLight_VP = 0;
static GLuint g_Selected_VP = 0;
static GLuint g_FragmentProgramID = 0;

bool InitResourceOpenGL(void)
{
	// `���Jpixel program, ���i�H�򤣦P������vertex shader�@��.`
	g_FragmentProgramID = GutLoadFragmentProgramOpenGL_ASM("../../shaders/vertex_lighting_directional.glpp");
	if ( g_FragmentProgramID==0 )
		return false;
	// `���Jdiretional light vertex program`
	g_DirLight_VP = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_lighting_directional.glvp");
	if ( g_DirLight_VP==0 )
		return false;
	// `���Jpoint light vertex program`
	g_PointLight_VP = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_lighting_point.glvp");
	if ( g_PointLight_VP==0 )
		return false;
	// `���Jspot light vertex program`
	g_SpotLight_VP = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_lighting_spot.glvp");
	if ( g_SpotLight_VP==0 )
		return false;

	// `�ഫ�����Y�y�Шt���x�}`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `�]�w��v�x�}`
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	// `��view_matrix��proj_matrix�ۭ�, �i�H���֤�᪺�p��.`
	g_view_proj_matrix = g_view_matrix * g_proj_matrix;
	// `�⥿�V��ϦV�������e�X��`
	glDisable(GL_CULL_FACE);
	// `�Ұ�zbuffer test`
	glEnable(GL_DEPTH_TEST);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_DirLight_VP )
	{
		GutReleaseVertexProgramOpenGL(g_DirLight_VP);
		g_DirLight_VP = 0;
	}

	if ( g_PointLight_VP )
	{
		GutReleaseVertexProgramOpenGL(g_PointLight_VP);
		g_PointLight_VP = 0;
	}

	if ( g_SpotLight_VP )
	{
		GutReleaseVertexProgramOpenGL(g_SpotLight_VP);
		g_SpotLight_VP = 0;
	}

	if ( g_FragmentProgramID )
	{
		GutReleaseVertexProgramOpenGL(g_FragmentProgramID);
		g_FragmentProgramID = 0;
	}

	return true;
}

// callback function. �����j�p���ܮɷ|�Q�I�s, �öǤJ�s�������j�p.
void ResizeWindowOpenGL(int width, int height)
{
	// �ϥηs�������j�p�����s��ø�ϸѪR��
	glViewport(0, 0, width, height);
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// ��view_matrix��proj_matrix�ۭ�, �i�H���֤�᪺�p��
	g_view_proj_matrix = g_view_matrix * g_proj_matrix;
}

static void SetupLightingOpenGL(void)
{
	int base = 12;
	Vector4 vShininess = g_fMaterialShininess;
	Vector4 vAmbient, vDiffuse, vSpecular, vSpotLightCoeff;
	// `�]�w���ҥ�`
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base, 1, &g_vAmbientLight[0]);
	
	//` �]�w����`
	switch(g_iSelectedLight)
	{
	// `��V��`
	default:
	case 0:
		g_Selected_VP = g_DirLight_VP;
		
		vAmbient = g_vMaterialAmbient * g_Lights[0].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[0].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[0].m_vSpecularColor;

		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 1, 1, &g_Lights[0].m_vDirection.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 2, 1, &vAmbient.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 3, 1, &vDiffuse.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 4, 1, &vSpecular.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 5, 1, &vShininess.x);

		break;
	// `�I����`
	case 1:
		g_Selected_VP = g_PointLight_VP;
		
		vAmbient = g_vMaterialAmbient * g_Lights[1].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[1].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[1].m_vSpecularColor;

		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 1, 1, &g_Lights[1].m_vPosition.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 2, 1, &g_Lights[1].m_vAttenuation.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 3, 1, &vAmbient.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 4, 1, &vDiffuse.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 5, 1, &vSpecular.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 6, 1, &vShininess.x);

		break;
	// `�E���O`
	case 2:
		g_Selected_VP = g_SpotLight_VP;
		
		vAmbient = g_vMaterialAmbient * g_Lights[2].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[2].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[2].m_vSpecularColor;

		float spotlight_cutoff_cosine = FastMath::Cos( FastMath::DegreeToRadian(g_Lights[2].m_fSpotlightCutoff * 0.5f) );
		vSpotLightCoeff.Set(spotlight_cutoff_cosine, g_Lights[2].m_fSpotlightExponent, 0.0f, 0.0f);

		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 1, 1, &g_Lights[2].m_vPosition.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 2, 1, &g_Lights[2].m_vDirection.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 3, 1, &g_Lights[2].m_vAttenuation.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 4, 1, &vSpotLightCoeff.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 5, 1, &vAmbient.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 6, 1, &vDiffuse.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 7, 1, &vSpecular.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 8, 1, &vShininess.x);

		break;
	}
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// `���w�n�ϥ�Shader�i�{���Ƭy�{`
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	// `�]�w����`	
	SetupLightingOpenGL();
	// `�]�w�ϥέ���Vertex��Fragment Program`
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, g_Selected_VP);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, g_FragmentProgramID);
	// `�]�w�n�ΰ}�C���覡�ǤJ���I��m���C��`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3N3), &g_pGridVertices[0].m_Position);
	glNormalPointer (GL_FLOAT, sizeof(Vertex_V3N3), &g_pGridVertices[0].m_Normal);
	// `�p��îM�J�ഫ�x�}`
	Matrix4x4 world_view_proj_matrix = g_world_matrix * g_view_proj_matrix; 
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, &world_view_proj_matrix[0][0]);
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 4, 4, &g_world_matrix[0][0]);
	// `���Y��m, �p��Specular�|�Ψ�.`
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 8, 1, &g_eye[0]);
	// `�e�X��l`
	glDrawElements (GL_TRIANGLE_STRIP, g_iNumGridIndices, GL_UNSIGNED_SHORT, g_pGridIndices);
	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
