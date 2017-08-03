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
float g_fFrame_Time = 0.0f;

static int g_iLightSource = LIGHT_DIRECTIONAL;
static int g_iNumGrids = 15;

void KeyDown_1(void)
{
	g_iLightSource = LIGHT_DIRECTIONAL;
	printf("Directional light\n");
}

void KeyDown_2(void)
{
	g_iLightSource = LIGHT_POINT;
	printf("Point light\n");
}

void KeyDown_3(void)
{
	g_iLightSource = LIGHT_SPOT;
	printf("Spot light\n");
}

void GetUserInput(void)
{
	//
	GutReadKeyboard();
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

		// �u���i�H�W�[/��ִѽL��l���ƥ�
		if ( mouse.z )
		{
			g_iNumGrids += mouse.z > 0 ? 1 : -1;
			
			if ( g_iNumGrids <=0 ) g_iNumGrids=1;
			if ( g_iNumGrids > g_iMaxNumGrids ) g_iNumGrids = g_iMaxNumGrids;

			ReleaseGridsResource(&g_pGridVertices, &g_pGridIndices);
			GenerateGrids(g_iNumGrids, g_iNumGrids, 
				&g_pGridVertices, g_iNumGridVertices, 
				&g_pGridIndices, g_iNumGridIndices,
				g_iNumGridTriangles);

			delete [] g_pGridVerticesDX9;
			g_pGridVerticesDX9 = new Vertex_DX9[g_iNumGridVertices];

			printf("Generate %dx%d grids\n", g_iNumGrids, g_iNumGrids);
		}
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
		resize_func = ResizeWindowDX9;
		break;
	case '2':
		device = "opengl";
		init_resource = InitResourceOpenGL;
		release_resource = ReleaseResourceOpenGL;
		render = RenderFrameOpenGL;
		resize_func = ResizeWindowOpenGL;
		break;
#ifdef _ENABLE_DX10_
	case '3':
		device = "dx10";
		init_resource = InitResourceDX10;
		release_resource = ReleaseResourceDX10;
		render = RenderFrameDX10;
		resize_func = ResizeWindowDX10;
		break;
#endif // _ENABLE_DX10_
	}

	printf("\nSelected %s device for rendering.\n", device);
	printf("Press\n(1) for point light.\n(2) for directional light.\n(3) for spot light.\n");

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

	GenerateGrids(g_iNumGrids, g_iNumGrids, 
		&g_pGridVertices, g_iNumGridVertices, 
		&g_pGridIndices, g_iNumGridIndices,
		g_iNumGridTriangles);

	g_pGridVerticesDX9 = new Vertex_DX9[g_iNumGridVertices];

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);

	// ���Jø�ϸ��
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	// `�]�w�������`
	g_vLightAmbient.Set(0.1f, 0.1f, 0.1f, 0.1f);
	g_vLightDirection.Set(0.0f, 0.0f, 1.0f);
	g_vLightPosition.Set(0.0f, 0.0f, 0.5f);
	g_vLightColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	g_vLightAttenuation.Set(1.0f, 1.0f, 0.0f);
	g_fSpotLightCutoff = 30.0f;
	g_fSpotLightExponent = 30.0f;

	// `�D�j��`
	while( GutProcessMessage() )
	{
		GetUserInput();
		// `��C�ӳ��I���C����]�w��g_vLightAmbient����`
		CalculateAmbientLight(g_pGridVertices, g_iNumGridVertices);
		// `�A�[�W�ҿ�ܪ�����`
		switch(g_iLightSource)
		{
		case LIGHT_DIRECTIONAL:
			CalculateDirectionalLight(g_pGridVertices, g_iNumGridVertices);
			break;
		case LIGHT_POINT:
			CalculatePointLight(g_pGridVertices, g_iNumGridVertices);
			break;
		case LIGHT_SPOT:
			CalculateSpotLight(g_pGridVertices, g_iNumGridVertices);
			break;
		}
		// render
		render();
	}
	
	// `����ѽL��ҫ��O����Ŷ�	`
	ReleaseGridsResource(&g_pGridVertices, &g_pGridIndices);
	delete [] g_pGridVerticesDX9;

	// ����ø�ϸ��
	release_resource();
	// ����OpenGL/DirectXø�ϸ˸m
	GutReleaseGraphicsDevice();
}
