#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	Matrix4x4 projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &g_view_matrix);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	ConvertToDX9(g_Quad, g_Quad_dx9, 4);
	ConvertToDX9(g_LightPosition, g_LightPosition_dx9, NUM_LIGHTS);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	GutResetGraphicsDeviceDX9();
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;
	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	Matrix4x4 projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// �e�X���V��ϦV���T����
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

static void SetupLighting(void)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�]�w���ҥ�`
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));
	// `���wDiffuse���誺��ƨӷ�, �⥦�]���g��SetMaterial�禡���ӳ]�w.`
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	// `�]�w������誺�ϥ���O`
	D3DCOLORVALUE vFullIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
	D3DCOLORVALUE vZeroIntensity = {0.0f, 0.0f, 0.0f, 0.0f};

	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Ambient = vFullIntensity;
	mtrl.Diffuse = vFullIntensity;
	mtrl.Specular = vFullIntensity;
	mtrl.Emissive = vZeroIntensity;
	mtrl.Power = g_fMaterialShininess;
	// `�I�sSetMaterial�ӳ]�w����`
	device->SetMaterial(&mtrl);
	
	// `��ҧ�쪺���ҥ��M�J�w�����`
	int i;
	for ( i=0; i<g_iNumActivatedLights; i++ )
	{
		device->LightEnable(i, TRUE);

		D3DLIGHT9 light;
		ZeroMemory( &light, sizeof(light) );
		
		light.Type = D3DLIGHT_POINT;
		light.Position = *(D3DVECTOR *) &g_ActivatedLights[i].m_Position;
		light.Diffuse = *(D3DCOLORVALUE*) &g_ActivatedLights[i].m_Diffuse;
		light.Specular = *(D3DCOLORVALUE*) &g_ActivatedLights[i].m_Specular;
		light.Range = 1000.0f;

		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;

		device->SetLight(i, &light);
	}

	// `��l���O����`
	for ( ; i<MAX_ACTIVATED_LIGHTS; i++ )
	{
		device->LightEnable(i, FALSE);
	}
}

// `�ϥ�Direct3D9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�����e��`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// `�}�l�Uø�ϫ��O`
	device->BeginScene();
	// `���I��Ʈ榡`
	device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
	// `�Ȯ����������\��`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	Matrix4x4 matIdentity; matIdentity.Identity();
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &matIdentity);
	// `�I���j�p`
	float fPointSize = 5.0f;
	// `��B�I�ƪ��O�����Ʊj���ഫ��DWORD���A`
	DWORD dwPointSize = *((DWORD*)&fPointSize);
	// `�]�w�e�I�ɨϥ�5x5�ӹ����ӵe�@���I`
	device->SetRenderState(D3DRS_POINTSIZE, dwPointSize);
	// `���I�ӵe�X������m`
	device->DrawPrimitiveUP(D3DPT_POINTLIST, NUM_LIGHTS, g_LightPosition_dx9, sizeof(Vertex_DX9));
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// `�]�w����`
	SetupLighting();
	// `���ʪ���`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
	// `�e�@���x�ΨӬݥ����ĪG`
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad_dx9, sizeof(Vertex_DX9));
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
