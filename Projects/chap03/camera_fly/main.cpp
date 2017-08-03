#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_opengl.h"
#include "render_data.h"

#ifdef _ENABLE_DX10_
#include "render_dx10.h"
#endif

GutTimer g_Timer;

void GetUserInput(void)
{
	// `Ū���ƹ�`
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// `Ū����L`
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);
	// `���o�e���e�@�ӵe����{�b�Ҹg�����ɶ�`
	float time_diff = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * time_diff;
	float rotation_speed = 1.0 * time_diff;

	// `���y�Шt��`
	static float theta = -MATH_PI * 0.5f;
	static float phi = 0.0f;

	// `�p�G���U�ƹ�����, �N�������Y.`
	if ( mouse.button[0] ) 
	{
		theta += mouse.x * rotation_speed;
		phi -= mouse.y * rotation_speed;
	}

	float sin_phi, cos_phi;
	float sin_theta, cos_theta;
	
	FastMath::SinCos(phi, sin_phi, cos_phi);
	FastMath::SinCos(theta, sin_theta, cos_theta);

	// `�p�����Y�����V`
	Vector4 camera_facing;

	camera_facing[0] = cos_phi * cos_theta;
	camera_facing[1] = sin_phi;
	camera_facing[2] = cos_phi * sin_theta;

	// `�p�����Y���W�誺�b�V`
	Vector4 camera_up;
	FastMath::SinCos(phi + MATH_PI*0.5f, sin_phi, cos_phi);

	camera_up[0] = cos_phi * cos_theta;
	camera_up[1] = sin_phi;
	camera_up[2] = cos_phi * sin_theta;

	// `���o�譱�k�誺��V`
	Vector4 camera_right = Vector3CrossProduct(camera_facing, camera_up);

	// `���UW�Τ�V��V�W`
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		g_eye += camera_facing * moving_speed;
	}
	// `���US�Τ�V��V�U`
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		g_eye -= camera_facing * moving_speed;
	}
	// `���UA�Τ�V��V��`
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
	{
		g_eye -= camera_right * moving_speed;
	}
	// `���UD�Τ�V��V�k`
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
	{
		g_eye += camera_right * moving_speed;
	}

	// `�p��X���Y��Ǫ��I, �������Y�ഫ�x�}�ɷ|�Ψ�.`
	g_lookat = g_eye + camera_facing;
	// `�]���O��2�Ӷb���, �ݭn��s���Y�¤W���b.`
	g_up = camera_up;
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

	g_view_matrix.Identity();

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
