#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;

void GetUserInput(void)
{
	// `Ū���ƹ�`
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// `Ū����L`
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);
	// `�p��ثe���Y���V`
	Vector4 facing = g_lookat - g_eye;
	// `�T�O���O�ӳ��V�q`
	facing.Normalize(); 
	// `���o�e���e�@�ӵe����{�b�Ҹg�����ɶ�`
	float time_diff = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * time_diff;
	float rotation_speed = 1.0 * time_diff;
	float ry = 0.0f;

	// `�p�G���U�ƹ�����, �N�������Y.`
	if ( mouse.button[0] ) 
		ry = mouse.x * rotation_speed;
	// `���UA�άO��V��<-, �N����.`
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
		ry = -rotation_speed;
	// `���UD�άO��V��->, �N�k��.`
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
		ry = rotation_speed;
	
	if ( ry )
	{
		// `�����o�@�ӱ���x�}`
		Matrix4x4 rotate_matrix;
		rotate_matrix.RotateY_Replace(-ry);
		// `��쥻�����V�uY�b����ry��, ���o�s�����V.`
		facing = facing * rotate_matrix;
	}

	// `���UW�Τ�V��V�W`
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		// `���Y�V�e����`
		g_eye += facing * moving_speed; 
	}

	// `���US�Τ�V��V�U`
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		// `���Y�V�Ჾ��`
		g_eye -= facing * moving_speed; 
	}

	// `�p��X���Y��Ǫ��I, �������Y�ഫ�x�}�ɷ|�Ψ�.`
	g_lookat = g_eye + facing; 
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

	GutInputInit();
	g_object_matrix.Identity();

	// ���Jshader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	// �D�j��
	while( GutProcessMessage() )
	{
		GetUserInput();
		render();
	}
	
	// ����shader
	release_resource();

	// ����OpenGL/DirectXø�ϸ˸m
	GutReleaseGraphicsDevice();
}
