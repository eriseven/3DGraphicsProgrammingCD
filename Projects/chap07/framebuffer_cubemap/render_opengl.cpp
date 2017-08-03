#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static CGutModel_OpenGL g_Models_OpenGL[4];

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

static GLuint g_texture = 0;
static GLuint g_depthtexture = 0;
static GLuint g_framebuffer = 0;
static GLuint g_depthbuffer = 0;

static GLuint g_framebuffers[6];
static GLuint g_textures[6];

bool InitResourceOpenGL(void)
{
	if ( glGenFramebuffersEXT==NULL )
	{
		printf("Could not create frame buffer object\n");
		return false;
	}

	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `��v�x�}`
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// `�]�w�����ഫ�x�}`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	glGenTextures(1, &g_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_texture);
	// `�]�w`filter
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint cubemap_id[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};


	// `�t�mzbuffer��framebuffer object�ϥ�`
	glGenRenderbuffersEXT(1, &g_depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 512, 512);

	int fail = 0;
	for ( int i=0; i<6; i++ )
	{
		glGenFramebuffersEXT(1, &g_framebuffers[i]);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffers[i]);
		// `�ŧi�K�Ϥj�p�ή榡`
		GLuint tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i;
		glTexImage2D(tex_target, 0, GL_RGBA8,  512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// `framebuffer��RGBAø��`
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, tex_target, g_texture, 0);
		// `6�Ӥ��P�����i�H�@�ΦP�@��depthbuffer`
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthbuffer);

		// `�ˬdframebuffer object���S���t�m���\`
		GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
		{
			fail++;
		}
	}

	// `�ˬdframebuffer object���S���t�m���\`
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CGutModel::SetTexturePath("../../textures/");
	
	for ( int i=0; i<4; i++ )
	{
		g_Models_OpenGL[i].ConvertToOpenGLModel(&g_Models[i]);
	}

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_framebuffer )
	{
		glDeleteFramebuffersEXT(1, &g_framebuffer);
		g_framebuffer = 0;
	}

	if ( g_depthbuffer )
	{
		glDeleteRenderbuffersEXT(1, &g_depthbuffer);
		g_depthbuffer = 0;
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
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// �]�w�����ഫ�x�}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

static void RenderSolarSystemOpenGL(Matrix4x4 &view_matrix)
{
	// �]�w�n�ΰ}�C���覡�ǤJ���I��m���C��
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	Matrix4x4 world_view_matrix;
	// �a�y
	world_view_matrix = g_earth_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[1].Render();
	// ��y
	world_view_matrix = g_moon_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[2].Render();
	// ���P
	world_view_matrix = g_mars_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[3].Render();
}

// �ϥ�OpenGL��ø��
void RenderFrameOpenGL(void)
{
	// `��s�ʺA�K��`
	{
		Matrix4x4 cubemap_perspective_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *)&cubemap_perspective_matrix);
		
		// `��scubemap���W�U���k�e��@6�ӭ�`
		for ( int i=0; i<6; i++ )
		{
			// `�ϥ�`g_framebuffer framebuffer object	
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffers[i]);
			glViewport(0, 0, 512, 512);
			// `�M���e��`
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// `���Y�n���藍�P����V`
			Matrix4x4 cubemap_view = GutMatrixLookAtRH(g_vCubemap_Eye, g_vCubemap_Lookat[i], -g_vCubemap_Up[i]);
			// `�e�X��P`
			RenderSolarSystemOpenGL(cubemap_view);
		}
	}
	// `�ϥΰʺA�K��`
	{
		// `�ϥΥDframebuffer object, �]�N�O����.`
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		int w, h;
		GutGetWindowSize(w, h);
		glViewport(0, 0, w, h);
		// `�M���e��`
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// `��۰ʭp��X�Ӫ��Ϯg�V�q�ഫ��@�ɮy�Шt`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 inv_view = g_Control.GetCameraMatrix();
		// `�u�O�d�x�}�����ೡ��`
		inv_view.NoTranslate();
		// `�]�w�K���ഫ�x�}`
		glActiveTexture(GL_TEXTURE1_ARB);
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf((float *)&inv_view);
		// `��v�x�}`
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);
		// `�ഫ�x�}`
		glMatrixMode(GL_MODELVIEW);
		Matrix4x4 world_view_matrix = g_sun_matrix * view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);
		// `��Model����3�h�K�ϱj���令cubemap`
		CGutModel_OpenGL::SetTextureOverwrite(2, g_texture);
		CGutModel_OpenGL::SetMapOverwrite(2, MAP_CUBEMAP);
		//` �e�Ӷ�`
		g_Models_OpenGL[0].Render();
		// `��Model����3�h�K���٭즨�ҫ���l�]�w`
		CGutModel_OpenGL::SetTextureOverwrite(2, 0);
		CGutModel_OpenGL::SetMapOverwrite(2, MAP_NOOVERWRITE);
		// `�e�䥦��P`
		RenderSolarSystemOpenGL(view_matrix);
	}

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
