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

void KeyDown_1(void)
{
	if ( g_iSwitch & 0x01 )
	{
		g_iSwitch &= (~0x01);
		printf("Lights 0-7 off\n");
	}
	else
	{
		g_iSwitch |= 0x01;
		printf("Lights 0-7 on\n");
	}
}

void KeyDown_2(void)
{
	if ( g_iSwitch & 0x02 )
	{
		g_iSwitch &= (~0x02);
		printf("Lights 8-15 off\n");
	}
	else
	{
		g_iSwitch |= 0x02;
		printf("Lights 8-15 on\n");
	}
}

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_FPSCAMERA);
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

	g_Model.SetTexturePath("../../textures/");
	g_Model.Load_ASCII("../../models/grand_canyon.gma");

	GutResizeFunc( resize_func );

	// �b(100,100)����m�}�Ҥ@�Ӥj�p��(512x512)������
	GutCreateWindow(100, 100, 512, 512, device);

	// ��OpenGL��DirectX��l��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	// �]�w��V��
	g_Light.m_Position.Set(0.0f, 1.0f, 1.0f, 0.0f);
	g_Light.m_Position.Normalize();
	g_Light.m_Diffuse.Set(0.6f, 0.6f, 0.6f, 1.0f);
	g_Light.m_Specular.Set(1.0f, 1.0f, 1.0f, 1.0f);

	// �]�w���ҥ�����m���C��
	Vector4 vFirstLight(-10.0f, -10.0f, 3.0f, 1.0f);
	Vector4 vLastLight ( 10.0f,  10.0f, 3.0f, 1.0f);
	Vector4 vSpan = vLastLight - vFirstLight;
	Vector4 vInc = vSpan / 3.0f;

	Vector4 vPos = vFirstLight;
	int i=0;
	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++, i++ )
		{
			g_Lights[i].m_Position = vPos;
			// �ζüƨӲ��ͥ����j��
			int r = rand() & 1023; // 0-1023�����ü�
			int g = rand() & 1023;
			int b = rand() & 1023;
			// ��r,g,b ���b0.5-1����
			g_Lights[i].m_Diffuse.Set(r/1024.0f, g/1024.0f, b/1024.0f, 1.0f);
			g_Lights[i].m_Specular.Set(0.0f, 0.0f, 0.0f, 1.0f);
			vPos[0] += vInc[0];
		}
		vPos[0] = vFirstLight[0];
		vPos[1] += vInc[1];
	}

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);

	//g_Control.SetCamera(Vector4(0.0f, -10.0f, 3.0f), Vector4(0.0f, 0.0f, 3.0f), Vector4(0.0f, 0.0f, 1.0f));
	g_Control.SetCamera(Vector4(0.0f, 0.0f, 30.0f), Vector4(0.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f));

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
	// ����OpenGL/DirectXø�ϸ˸m
	GutReleaseGraphicsDevice();
}
