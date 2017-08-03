#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

void GetUserInput(void)
{
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	
	if ( mouse.button[0] )
	{
		// �p�G�����U����, �N���ફ��
		float rz = mouse.x * 0.01f;
		float rx = mouse.y * -0.01f;

		Matrix4x4 rotation_matrix;
		rotation_matrix.RotateZ_Replace(rz);
		rotation_matrix.RotateX(rx);
	
		g_object_matrix = g_object_matrix * rotation_matrix;
	}

	if ( mouse.z )
	{
		// �p�G�u�����b��, �N�ϥκu�����Y�񪫥�
		float scale = 1.0f + mouse.z * 0.001f;
		g_object_matrix.Scale(scale, scale, scale);
	}
}

void main(int argc, char *argv[])
{
	// ���w�ϥ�DirectX 9��ø��
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;

#ifdef _ENABLE_DX10_
	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n(3) for Direct3D10\n");
#else
	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n");
#endif

	int c = getche();

	switch(c)
	{
	default:
	case '1':
		render = RenderFrameDX9;
		init_resource = InitResourceDX9;
		release_resource = ReleaseResourceDX9;
		break;
	case '2':
		device = "opengl";
		init_resource = InitResourceOpenGL;
		release_resource = ReleaseResourceOpenGL;
		render = RenderFrameOpenGL;
		break;
	case '3':
	#ifdef _ENABLE_DX10_
		device = "dx10";
		init_resource = InitResourceDX10;
		release_resource = ReleaseResourceDX10;
		render = RenderFrameDX10;
	#endif
		break;
	}

	// �b(100,100)����m�}�Ҥ@�Ӥj�p��(512x512)������
	GutCreateWindow(100, 100, 512, 512, device);

	// ��OpenGL��DirectX��l��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}


	// ���Jshader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	// ��l��DirectX Input����
	GutInputInit();
	// ����x�}�]�����x�}
	g_object_matrix.Identity();

	// �D�j��
	while( GutProcessMessage() )
	{
		// Ū���ƹ�, �íp��s��������Y��x�}
		GetUserInput();
		// �e��
		render();
	}
	
	// ����shader
	release_resource();

	// ����OpenGL/DirectXø�ϸ˸m
	GutReleaseGraphicsDevice();
}
