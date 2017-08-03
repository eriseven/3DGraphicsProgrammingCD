#include "Gut.h"
#include "GutModel_DX9.h"
#include "render_data.h"

#include "GutWin32.h"

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_view_matrix;
static CGutModel_DX9 g_Models_DX9[3];

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(70.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for ( int i=0; i<3; i++ )
	{
		g_Models_DX9[i].ConvertToDX9Model(&g_Models[i]);
	}

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(70.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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

void SetupLightingDX9(Light *pLightArray, int num_lights)
{
	// ���oDirect3D9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �]�w���ҥ�
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));

	int i;

	for ( i=0; i<num_lights; i++ )
	{
		// �]�w����
		device->LightEnable(i, TRUE);

		D3DLIGHT9 light;
		ZeroMemory( &light, sizeof(light) );
		
		light.Type = D3DLIGHT_POINT;

		light.Position = *(D3DVECTOR *) &pLightArray[i].m_Position;
		light.Diffuse = *(D3DCOLORVALUE*) &pLightArray[i].m_Diffuse;
		light.Specular = *(D3DCOLORVALUE*) &pLightArray[i].m_Specular;
		light.Range = 1000.0f;

		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;

		device->SetLight(i, &light);
	}
}


void RenderSolarSystemDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �Ӷ�
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_sun_matrix);
	g_Models_DX9[0].Render();
	//device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �a�y
	//SetupLightingDX9(&g_Light, 1);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_earth_matrix);
	g_Models_DX9[1].Render();
	// ��G
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_moon_matrix);
	g_Models_DX9[2].Render();
}
// �ϥ�Direct3D9��ø��
void RenderFrameDX9(void)
{
	// `���o�����j�p`
	int w, h;
	GutGetWindowSize(w, h);
	// `�M���e��`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 100, 100, 255), 1.0f, 0);
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 

	Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	Matrix4x4 ortho_proj = GutMatrixOrthoRH_DirectX(20.0f, 20.0f, 0.1f, 100.0f);

	{
		// `�e����`
		D3DVIEWPORT9 viewport = {0, 0, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Vector4 camera_pos(0.0f, -20.0f, 0.0f);
		Vector4 camera_up(0.0f, 0.0f, 1.0f);
		g_view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ortho_proj);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		// `�W����`
		D3DVIEWPORT9 viewport = {w/2, 0, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Vector4 camera_pos(0.0f, 0.0f, 20.0f);
		Vector4 camera_up(0.0f, 1.0f, 0.0f);
		g_view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ortho_proj);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		// `�k����`
		D3DVIEWPORT9 viewport = {0, h/2, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Vector4 camera_pos(20.0f, 0.0f, 0.0f);
		Vector4 camera_up(0.0f, 0.0f, 1.0f);
		g_view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ortho_proj);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		// `�ϥΪ̵���`
		D3DVIEWPORT9 viewport = {w/2, h/2, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
		g_view_matrix = object_matrix * view_matrix;
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&g_projection_matrix);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		D3DVIEWPORT9 viewport = {0, 0, w, h, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Matrix4x4 ident_mat; ident_mat.Identity();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&ident_mat);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&ident_mat);
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ident_mat);
		// render border
		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_LINELIST, 2, g_Border, sizeof(Vertex_VC));
	}
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
