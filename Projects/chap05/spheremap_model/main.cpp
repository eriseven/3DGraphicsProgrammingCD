#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"
#include "GutModel.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;
float g_fFrame_Time = 0.0f;
bool g_bFPS_Camera = false;
static char g_keyboard_state[256];

void GetUserInput_FPSCamera(void)
{
	// Ū���ƹ�
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// Ū����L
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);

	// ���o�e���e�@�ӵe����{�b�Ҹg�����ɶ�
	float time_diff = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * time_diff;
	float rotation_speed = 1.0 * time_diff;

	// �n�ֿn�U��X��Y�b�����ਤ��
	static float ry = 0.0f;
	static float rx = 0.0f;

	// �p�G���U�ƹ�����A�N�������Y
	if ( mouse.button[0] ) 
	{
		ry += mouse.x * rotation_speed;
		rx -= mouse.y * rotation_speed;
	}

	Matrix4x4 rotate_matrix;
	rotate_matrix.RotateY_Replace(ry);
	rotate_matrix.RotateX(rx);

	// �b�k��y�Шt��, �譱�����V�O�ഫ�x�}��-Z�b
	Vector4 camera_facing = -rotate_matrix[2];
	// ���o�譱�k�䪺��V
	Vector4 camera_right = rotate_matrix[0];

	// ���UW�Τ�V��V�W
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		g_eye += camera_facing * moving_speed;
	}
	// ���US�Τ�V��V�U
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		g_eye -= camera_facing * moving_speed;
	}
	// ���UA�Τ�V��V��
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
	{
		g_eye -= camera_right * moving_speed;
	}
	// ���UD�Τ�V��V�k
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
	{
		g_eye += camera_right * moving_speed;
	}

	// �p��X���Y��Ǫ��I, �������Y�ഫ�x�}�ɷ|�Ψ�.
	g_lookat = g_eye + camera_facing;
	// �]���O��2�Ӷb���, �ݭn��s���Y�¤W���b
	g_up = Vector3CrossProduct(camera_right, camera_facing);
	g_up.Normalize();

}

void GetUserInput_RotateObject(void)
{
	// Ū���ƹ�
	GutMouseInfo mouse;
	if ( GutReadMouse(&mouse) )
	{
		// ���o�e���e�@�ӵe����{�b�Ҹg�����ɶ�
		g_fFrame_Time = g_Timer.Stop();
		g_Timer.Restart();

		float moving_speed = 2.0f * g_fFrame_Time;
		float rotation_speed = 1.0 * g_fFrame_Time;

		// �p�G���U�ƹ�����A�N�������Y
		if ( mouse.button[0] ) 
		{
			Matrix4x4 rotate_matrix;
			rotate_matrix.RotateY_Replace(mouse.x * rotation_speed);
			rotate_matrix.RotateX(mouse.y * rotation_speed);
			g_world_matrix = g_world_matrix * rotate_matrix;
		}

		if ( mouse.z )
		{
			float scale = 1.0f + mouse.z / 1000.0f;
			g_world_matrix.Scale(scale, scale, scale);
		}
	}
}

void GetUserInput(void)
{
	if ( g_bFPS_Camera )
	{
		GetUserInput_FPSCamera();
	}
	else
	{
		GetUserInput_RotateObject();
	}
}

void main(void)
{
	// ���w�ϥ�DirectX 9��ø��
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;
	void (*resize_func)(int width, int height) = ResizeWindowDX9;

	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n");
	int c = getche();
	switch(c)
	{
	default:
	case '1':
		render = RenderFrameDX9;
		init_resource = InitResourceDX9;
		release_resource = ReleaseResourceDX9;
		resize_func = ResizeWindowDX9;
		break;
	case '2':
		device = "opengl";
		init_resource = InitResourceOpenGL;
		release_resource = ReleaseResourceOpenGL;
		render = RenderFrameOpenGL;
		resize_func = ResizeWindowOpenGL;
		break;
	}

	printf("\nSelected %s device for rendering.\n", device);

	char *model = "..\\..\\models\\jet.gma";

	/*
	printf("Press\n");
	printf("(1) to load a box\n");
	printf("(2) to load a jet fighter\n");
	printf("(3) to load a sport car\n");
	printf("(4) to load a keyboard\n");
	printf("(5) to load a sphere\n");

	c = getche();
	printf("\n");

	switch(c)
	{
	default:
	case '1':
		model = "..\\..\\models\\box.gma";
		break;
	case '2':
		model = "..\\..\\models\\jet.gma";
		break;
	case '3':
		model = "..\\..\\models\\sportcar.gma";
		break;
	case '4':
		model = "..\\..\\models\\keyboard.gma";
		break;
	case '5':
		model = "..\\..\\models\\sphere.gma";
		break;
	}
	*/

	// ���J�ҫ�
	g_Model.Load_ASCII(model);

	GutResizeFunc( resize_func );

	// �b(100,100)����m�}�Ҥ@�Ӥj�p��(512x512)������
	GutCreateWindow(100, 100, 512, 512, device);

	// ��OpenGL��DirectX��l��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	g_view_matrix.Identity();
	g_world_matrix.Identity();

	GutInputInit();

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
		// �e�X�x��
		render();
	}
	// ����shader
	release_resource();
	//
	// ����OpenGL/DirectXø�ϸ˸m
	GutReleaseGraphicsDevice();
}
