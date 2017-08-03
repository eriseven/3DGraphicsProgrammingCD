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
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
	//g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_FPSCAMERA);
}

void KeyDown_Space(void)
{
	g_bDrawShadowVolume = !g_bDrawShadowVolume;
	printf("%s\n", g_bDrawShadowVolume ? "Draw ShadowVolume" : "Do not draw ShadowVolume"); 
}

void KeyDown_1(void)
{
	g_bDirectionalLight = !g_bDirectionalLight;
	printf("%s\n", g_bDirectionalLight ? "Directional Light" : "Point Light"); 
}

void main(void)
{

	// ���w�ϥ�DirectX 9��ø��
	char *device = "dx10";
	void (*render)(void) = RenderFrameDX10;
	bool (*init_resource)(void) = InitResourceDX10;
	bool (*release_resource)(void) = ReleaseResourceDX10;
	void (*resize_func)(int width, int height) = ResizeWindowDX10;

	GutResizeFunc( resize_func );

	// �b(100,100)����m�}�Ҥ@�Ӥj�p��(512x512)������
	GutCreateWindow(100, 100, 512, 512, device);

	// ��OpenGL��DirectX��l��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	GutInputInit();
	GutRegisterKeyDown(GUTKEY_SPACE, KeyDown_Space);
	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);

	g_Box_shadowvolume.Load_ASCII("../../models/box.gma");
	g_Box.Load_ASCII("../../models/box_brick.gma");
	//g_Box_shadowvolume.Load_ASCII("../../models/donut_v.gma");
	//g_Box.Load_ASCII("../../models/donut.gma");
	g_Wall.Load_ASCII("../../models/wall.gma");

	g_Control.SetCamera(Vector4(-7.0f, 0.0f, 7.0f), Vector4(-2.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	g_ShadowVolume.Preprocess(g_Box_shadowvolume);
	g_ShadowVolume.Preprocess_Adj();
	
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
