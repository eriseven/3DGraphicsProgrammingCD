#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx10.h"
#include "render_data.h"

GutTimer g_Timer;
float g_fFrame_Time = 0.0f;

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
}

void KeyDown_1(void)
{
	g_iMode = 1;
	printf("sin curve\n");
}

void KeyDown_2(void)
{
	g_iMode = 2;
	printf("z curve\n");
}

void main(void)
{
	// ���w�ϥ�DirectX 9��ø��
	char *device = "dx10";
	void (*render)(void) = RenderFrameDX10;
	bool (*init_resource)(void) = InitResourceDX10;
	bool (*release_resource)(void) = ReleaseResourceDX10;
	void (*resize_func)(int width, int height) = ResizeWindowDX10;

	GutResizeFunc(resize_func);
	// �b(100,100)����m�}�Ҥ@�Ӥj�p��(512x512)������
	GutCreateWindow(100, 100, 512, 512, device);
	// D3D��l��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);

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
