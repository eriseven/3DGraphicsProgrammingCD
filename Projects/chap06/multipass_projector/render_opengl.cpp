#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"
#include "GutModel_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_projection_matrix;

GLuint g_texture = 0;

CGutModel_OpenGL g_Model_OpenGL;

bool InitResourceOpenGL(void)
{
	// ���Y�y�Шt�ഫ�x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// ��v�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	CGutModel::SetTexturePath("../../textures/");
	
	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	g_texture = GutLoadTexture_OpenGL("../../textures/nasa_stars.tga");

	if ( g_texture==0 )
		return false;

	// �⥿�V��ϦV�������e�X��
	glDisable(GL_CULL_FACE);
	// �Ұ�2D�K�ϥ\��
	glEnable(GL_TEXTURE_2D);
	// �Ұ�zbuffer���í�����
	glEnable(GL_DEPTH_TEST);
	//
	glDepthFunc(GL_LEQUAL);

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

void SetupLightingOpenGL(void)
{
	int LightID = GL_LIGHT0;

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(LightID);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &g_vAmbientLight[0]);

	glLightfv(LightID, GL_POSITION, &g_Light.m_Position[0]);
	glLightfv(LightID, GL_DIFFUSE, &g_Light.m_Diffuse[0]);
	glLightfv(LightID, GL_SPECULAR, &g_Light.m_Specular[0]);

	glLightf(LightID, GL_CONSTANT_ATTENUATION,	1.0f);
	glLightf(LightID, GL_LINEAR_ATTENUATION,	0.0f);
	glLightf(LightID, GL_QUADRATIC_ATTENUATION, 0.0f);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;

	{
		// `�e�X�����ѽL`
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);
		SetupLightingOpenGL();
		glLoadMatrixf( (float *) &world_view_matrix);
		if ( g_bPass0 )
			g_Model_OpenGL.Render();
	}

	{
		// `��v��
		sModelMaterial_OpenGL material;
		material.m_bBlend = true;
		material.m_SrcBlend = GL_ONE;
		material.m_DestBlend = GL_ONE;
		material.m_Textures[0] = g_texture;
		material.Submit(NULL);

		// `�Ұʦ۰ʲ��ͶK�Ϯy�Х\��`
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		// `�]�w���ѦҪ����m�Ӳ��ͶK�Ϯy��`
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		//
		Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();
		glTexGenfv(GL_S, GL_OBJECT_PLANE, &ident_matrix[0][0]);
		glTexGenfv(GL_T, GL_OBJECT_PLANE, &ident_matrix[1][0]);
		glTexGenfv(GL_R, GL_OBJECT_PLANE, &ident_matrix[2][0]);
		//
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

		// `�p��`texture matrix
		Vector4 vLightPos = g_Light.m_Position;
		Vector4 vLightUp(0.0f, 1.0f, 0.0f);
		Vector4 vLightLookat = g_Light.m_LookAt;
		// `��v����m�����Y�y�Шt�ഫ�x�}`
		Matrix4x4 light_view_matrix = GutMatrixLookAtRH(vLightPos, vLightLookat, vLightUp);
		// `��v�����Y����v�ഫ�x�}`
		Matrix4x4 light_projection_matrix = GutMatrixPerspectiveRH_OpenGL(30.0f, 1.0f, 0.1f, 100.0f);
		// `�� -1~1 �ഫ�� 0~1 �d�򪺯x�}`
		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Scale_Replace(0.5f, -0.5f, 1.0f); // invert Y
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.0f, 1.0f);
		// `�o�N�O�̫�n�ϥΪ�`texture matrix
		Matrix4x4 texture_matrix = g_world_matrix * light_view_matrix * light_projection_matrix * uv_offset_matrix;
		// `�M��`texture matrix
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf( (float *)&texture_matrix);
		
		if ( g_bPass1 )
			// `�ǤJ0�N���ϥΥ��i�ҫ���������]�w`
			g_Model_OpenGL.Render(0);

		glLoadIdentity();

		glDisable(GL_LIGHTING);
	}

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
