#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutOpenGL.h"
#include "GutWin32.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static GLuint g_texture = 0;

static GLuint g_BlurVS = 0;
static GLuint g_BlurPS = 0;
static GLuint g_BlurProgram = 0;

static GLuint g_BrightnessPS = 0;
static GLuint g_BrightnessProgram = 0;

static sImageInfo g_ImageInfo;

static GLuint g_framebuffer[2] = {0, 0};
static GLuint g_frametexture[2] = {0, 0};

// One time Init
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

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_TEXTURE_2D);

	g_texture = GutLoadTexture_OpenGL("../../textures/space.tga", &g_ImageInfo);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if ( g_texture==0 )
		return false;

	// ���JVertex Shader
	g_BlurVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect_blur.glvs");
	// ���JPixel Shader
	g_BlurPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_blur.glfs");
	if ( 0==g_BlurVS || 0==g_BlurPS )
		return false;
	// �إ�Shader Program����
	g_BlurProgram = glCreateProgram();
	if ( 0==g_BlurProgram )
		return false;
	// ��Vertex Shader�M�Jg_BlurProgram��
	glAttachShader(g_BlurProgram, g_BlurVS);
	// ��Pixel Shader�M�Jg_BlurProgram��
	glAttachShader(g_BlurProgram, g_BlurPS);
	// ��g_BlurProgram����Vertex Shader��Pixel Shader�s���_��
	glLinkProgram(g_BlurProgram);

	g_BrightnessPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_brightness.glfs");
	if ( 0==g_BrightnessPS )
		return false;

	g_BrightnessProgram = GutCreateProgram(g_BlurVS, g_BrightnessPS);
	if ( 0==g_BrightnessProgram )
		return false;

	int w = g_ImageInfo.m_iWidth/4;
	int h = g_ImageInfo.m_iHeight/4;

	if ( !GutCreateRenderTargetOpenGL(w, h, GL_RGBA8, &g_framebuffer[0], &g_frametexture[0]) )
		return false;

	if ( !GutCreateRenderTargetOpenGL(w, h, GL_RGBA8, &g_framebuffer[1], &g_frametexture[1]) )
		return false;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_texture )
	{
		glDeleteTextures(1, &g_texture);
		g_texture = 0;
	}

	glDeleteShader(g_BlurVS);
	glDeleteShader(g_BlurPS);
	glDeleteProgram(g_BlurProgram);

	glDeleteShader(g_BrightnessPS);
	glDeleteShader(g_BrightnessProgram);

	for ( int i=0; i<2; i++ )
	{
		glDeleteFramebuffersEXT(1, &g_framebuffer[i]);
		glDeleteTextures(1, &g_frametexture[i]);
	}

	return true;
}

// callback function. �����j�p���ܮɷ|�Q�I�s, �öǤJ�s�������j�p.
void ResizeWindowOpenGL(int width, int height)
{
	// �ϥηs�������j�p�����s��ø�ϸѪR��
	glViewport(0, 0, width, height);
}

static GLuint Brightness(GLuint texture, sImageInfo *pInfo)
{
	int w = pInfo->m_iWidth/4;
	int h = pInfo->m_iHeight/4;
	// `��ʺA�K�Ϩӵe��`
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer[1]);
	glViewport(0, 0, w, h);
	// `�ϥι�G�װ��ܤƪ�Shader`
	glUseProgram(g_BrightnessProgram);
	// `�M�ζK��`
	glBindTexture(GL_TEXTURE_2D, texture);
	// `�]�w�G���ܤưѼ�`
	GLint reg_offset = glGetUniformLocation(g_BrightnessProgram, "IntensityOffset");
	GLint reg_scale = glGetUniformLocation(g_BrightnessProgram, "IntensityScale");
	glUniform4fv(reg_offset, 1, (float *)&g_vBrightnessOffset);
	glUniform4fv(reg_scale, 1, (float *)&g_vBrightnessScale);
	// `�]�w��Ƹ�Ʈ榡`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuadInv[0].m_Texcoord);
	// 
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return g_frametexture[1];
}

static GLuint BlurImage(GLuint texture, sImageInfo *pInfo)
{	
	int w = pInfo->m_iWidth/4;
	int h = pInfo->m_iHeight/4;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;
	
	const int num_samples = KERNELSIZE;

	Vector4 vTexOffsetX[num_samples];
	Vector4 vTexOffsetY[num_samples];

	for ( int i=0; i<num_samples; i++ )
	{
		vTexOffsetX[i].Set(g_uv_offset_table[i] * fTexelW, 0.0f, 0.0f, g_weight_table[i]);
		vTexOffsetY[i].Set(0.0f, g_uv_offset_table[i] * fTexelH, 0.0f, g_weight_table[i]);
	}

	glUseProgram(g_BlurProgram);

	GLint reg = glGetUniformLocation(g_BlurProgram, "vTexOffset");

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuadInv[0].m_Texcoord);

	// `������V�ҽk`
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer[0]);
		glViewport(0, 0, w, h);
		glBindTexture(GL_TEXTURE_2D, texture);
		if ( reg>=0 )
			glUniform4fv(reg, num_samples, (float *)vTexOffsetX);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `������V�ҽk`
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer[1]);
		glBindTexture(GL_TEXTURE_2D, g_frametexture[0]);
		if ( reg>=0 )
			glUniform4fv(reg, num_samples, (float *)vTexOffsetY);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	return g_frametexture[1];
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	int w, h;
	GutGetWindowSize(w, h);
	glViewport(0, 0, w, h);
	// `�M���e��`
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `���ϥ�` Shader
	glUseProgram(0);
	// `�]�w����, �M�ζK��.`
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	// `�]�w���I��Ʈ榡`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Texcoord);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if ( g_bPosteffect )
	{
		// `���X�Ϥ������G������`
		GLuint texture = Brightness(g_texture, &g_ImageInfo);
		// `��Ϥ����ҽk��`
		texture = BlurImage(texture, &g_ImageInfo);
		// `�ϥΥD�e��`
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glViewport(0, 0, w, h);
		// `���ϥ�` Shader
		glUseProgram(0);
		// `�]�w����, �M�ΰʺA�K��.`
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, texture);
		// `�ҰʲV��\��`
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		// `�]�w���I��Ʈ榡`
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Texcoord);
		// `�e�X�ݪO`
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// `�����V��`
		glDisable(GL_BLEND);
	}
	
	GutSwapBuffersOpenGL();
}
