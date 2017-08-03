#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"

static Matrix4x4 g_proj_matrix;

static CGutModel_OpenGL g_Model_OpenGL;

static GLuint g_depthtexture = 0;
static GLuint g_depthbuffer = 0;

static GLuint g_ZPassVS = 0;
static GLuint g_ZPassPS = 0;
static GLuint g_ZPassProgram = 0;

static GLuint g_ShadowmapVS = 0;
static GLuint g_ShadowmapPS = 0;
static GLuint g_ShadowmapZComparePS = 0;
static GLuint g_ShadowmapProgram = 0;
static GLuint g_ShadowmapZCompareProgram = 0;

static Matrix4x4 g_light_matrix, g_remap_matrix;
static Matrix4x4 g_light_proj_matrix, g_light_view_matrix;

static bool g_bUseZCompare = false;

bool InitResourceOpenGL(void)
{
	if ( NULL==glCreateProgram )
	{
		printf("Driver does not support GLSL\n");
		return false;
	}
	if ( NULL==glBindFramebufferEXT )
	{
		printf("Driver does not support framebuffer object\n");
		return false;
	}

	// ��v�x�}
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);

	CGutModel::SetTexturePath("../../textures/");
	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	GutCreateRenderTargetOpenGL(shadowmap_size, shadowmap_size, &g_depthbuffer, 
		0, NULL, 
		GL_DEPTH_COMPONENT24, &g_depthtexture);

	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer (GL_TRUE);
	glReadBuffer (GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, g_depthtexture);

	if ( g_bUseZCompare )
	{
		// �Ұʦ۰ʤ�����\��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE_ARB);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	}
	else
	{
		// �����۰ʤ�����\��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	}

	// �eShadowmap��Shader
	// ���JVertex Shader
	g_ZPassVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/ZPass.glvs");
	// ���JPixel Shader
	g_ZPassPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/ZPass.glfs");
	if ( 0==g_ZPassVS || 0==g_ZPassPS )
		return false;
	// �ئ�Shader Program���� 
	g_ZPassProgram = glCreateProgram();
	// ��Vertex Shader�M�Jg_ZPassProgram��
	glAttachShader(g_ZPassProgram, g_ZPassVS);
	// ��Pixel Shader�M�Jg_ZPassProgram��
	glAttachShader(g_ZPassProgram, g_ZPassPS);
	// ��g_ZPassProgram�Ҥ��t�쪺Vertex Shader��Pixel Shader�s���_��
	glLinkProgram(g_ZPassProgram);

	g_ZPassProgram = GutCreateProgram(g_ZPassVS, g_ZPassPS);

	// �e���v�ĪG�Ϊ�Shader
	g_ShadowmapVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Shadowmap.glvs");
	g_ShadowmapPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/Shadowmap.glfs");
	g_ShadowmapZComparePS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/ShadowmapZCompare.glfs");
	if ( 0==g_ShadowmapVS || 0==g_ShadowmapPS || 0==g_ShadowmapZComparePS )
		return false;

	g_ShadowmapProgram = GutCreateProgram(g_ShadowmapVS, g_ShadowmapPS);
	g_ShadowmapZCompareProgram = GutCreateProgram(g_ShadowmapVS, g_ShadowmapZComparePS);

	// �����ഫ�x�}
	g_light_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fLightFOV, 1.0f, g_fLightNearZ, g_fLightFarZ);
	g_light_view_matrix = GutMatrixLookAtRH(g_vLightPos, g_vLightLookAt, g_vLightUp);
	
	// �ഫ��K�Ϯy�Шt���x�}
	g_remap_matrix.Identity();
	g_remap_matrix.Scale(0.5f, 0.5f, 0.5f);
	g_remap_matrix[3].Set(0.5f, 0.5f, 0.5f, 1.0f);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	glDeleteTextures(1, &g_depthtexture);
	glDeleteFramebuffersEXT(1, &g_depthbuffer);

	glDeleteShader(g_ZPassVS);
	glDeleteShader(g_ZPassPS);
	glDeleteProgram(g_ZPassProgram);

	glDeleteShader(g_ShadowmapVS);
	glDeleteShader(g_ShadowmapPS);
	glDeleteShader(g_ShadowmapZComparePS);

	glDeleteProgram(g_ShadowmapProgram);
	glDeleteProgram(g_ShadowmapZCompareProgram);

	return true;
}

// callback function. �����j�p���ܮɷ|�Q�I�s, �öǤJ�s�������j�p.
void ResizeWindowOpenGL(int width, int height)
{
	// �ϥηs�������j�p�����s��ø�ϸѪR��
	glViewport(0, 0, width, height);
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);
}

static void RenderShadowmap(void)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_depthbuffer);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, shadowmap_size, shadowmap_size);
	// �M���e��
	glClear(GL_DEPTH_BUFFER_BIT);
	//
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 light_wv_matrix = world_matrix * g_light_view_matrix;
	Matrix4x4 light_wvp_matrix = world_matrix * g_light_view_matrix * g_light_proj_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_light_proj_matrix );
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &light_wv_matrix );

	CGutModel_OpenGL::SetShaderOverwrite(g_ZPassProgram);
	g_Model_OpenGL.Render();
	CGutModel_OpenGL::SetShaderOverwrite(0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glPopAttrib();
	glDrawBuffer(GL_TRUE);
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	//
	RenderShadowmap();
	// �M���e��
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// �ഫ�x�}
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;

	Matrix4x4 zbias; zbias.Translate_Replace(0.0f, 0.0f, g_fZBias);

	Matrix4x4 light_wvp_matrix = world_matrix * g_light_view_matrix * zbias * g_light_proj_matrix * g_remap_matrix;
	Matrix4x4 light_wv_matrix = world_matrix * g_light_view_matrix;
	light_wv_matrix[3].Set(0.0f, 0.0f, 0.0f, 1.0f);
	// ��s�ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// ������ഫ�x�}���texture matrix��
	glMatrixMode(GL_TEXTURE); 
	glActiveTexture(GL_TEXTURE0_ARB);
	glLoadMatrixf( (float *) &light_wvp_matrix );
	glActiveTexture(GL_TEXTURE1_ARB);
	glLoadMatrixf( (float *) &light_wv_matrix );
	glActiveTexture(GL_TEXTURE0_ARB);

	CGutModel_OpenGL::SetTextureOverwrite(0, g_depthtexture);
	if ( g_bUseZCompare )
	{
		CGutModel_OpenGL::SetShaderOverwrite(g_ShadowmapZCompareProgram);
	}
	else
	{
		CGutModel_OpenGL::SetShaderOverwrite(g_ShadowmapProgram);
	}
	g_Model_OpenGL.Render();
	CGutModel_OpenGL::SetShaderOverwrite(0);

	// ��I��backbuffer���e���e�{�X��
	GutSwapBuffersOpenGL();
}
