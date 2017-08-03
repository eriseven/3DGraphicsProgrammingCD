#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutFont_DX9.h"
#include "GutHeightmap_DX9.h"

#include "render_data.h"

static CGutFontDX9 g_Font;
static CGutHeightmapDX9 g_Heightmap;

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(20, 30);
	g_Font.SetConsoleSize(32, 16);
	// ���J�r��
	g_Font.LoadTexture("../../textures/ascii_couriernew.tga");
	// ���Jheightmap
	if ( !g_Heightmap.LoadHeightmapTexture("../../textures/GrandCanyon.tga") )
		return false;
	g_Heightmap.SetRange(Vector4(-50.0f, -50.0f, -5.0f), Vector4(50.0f, 50.0f, 10.0f));

	D3DCAPS9 caps;
	device->GetDeviceCaps(&caps);

	// �Y���ª��w��@�f�𤣯�e�Ӧh���I
	int verts = caps.MaxPrimitiveCount < 65536 ? 100 : 255;

	if ( !g_Heightmap.BuildMesh(verts, verts) )
		return false;

	Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.5f, 1000.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&proj_matrix);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 	
	g_Font.SetConsoleResolution(width, height);
	g_Font.BuildMesh();

	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &proj_matrix);
}

D3DCOLOR ConvertToD3DCOLOR(Vector4 &vColor)
{
	Vector4 vColor_0_255 = vColor;
	vColor_0_255.Saturate();
	vColor_0_255 *= 255.0f;

	int r = (int) vColor_0_255.GetX();
	int g = (int) vColor_0_255.GetY();
	int b = (int) vColor_0_255.GetZ();
	int a = (int) vColor_0_255.GetW();

	D3DCOLOR d3dcolor = D3DCOLOR_ARGB(a, r, g, b);
	return d3dcolor;
}
//
static void SetupLighting(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �]�w���ҥ�
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));
	// ���wDiffuse���誺��ƨӷ�, �⥦�]���g��SetMaterial�禡���ӳ]�w
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	D3DCOLORVALUE vFullIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
	D3DCOLORVALUE vZeroIntensity = {0.0f, 0.0f, 0.0f, 0.0f};

	// �]�w������誺�ϥ���O

	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Ambient = vFullIntensity;
	mtrl.Diffuse = vFullIntensity;
	mtrl.Specular = vFullIntensity;
	mtrl.Emissive = vZeroIntensity;
	mtrl.Power = g_fMaterialShininess;

	// �I�sSetMaterial�ӳ]�w����.
	device->SetMaterial(&mtrl);
	// �]�w����
	device->LightEnable(0, TRUE);
	//
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(light) );
	
	light.Type = D3DLIGHT_DIRECTIONAL;

	Vector4 LightDir = -g_Lights[0].m_Direction;
	light.Direction = *(D3DVECTOR *) &LightDir;
	light.Diffuse = *(D3DCOLORVALUE*) &g_Lights[0].m_Diffuse;
	light.Specular = *(D3DCOLORVALUE*) &g_Lights[0].m_Specular;
	light.Range = 1000.0f;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	device->SetLight(0, &light);
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �����e��
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 
	
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&world_matrix);
	//
	SetupLighting();
	//
	device->SetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	g_Heightmap.Render();
	//
	device->EndScene(); 
	// ��I��backbuffer�e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
