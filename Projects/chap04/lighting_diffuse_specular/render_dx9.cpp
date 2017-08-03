#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// �e�X���V��ϦV���T����
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	GutResetGraphicsDeviceDX9();
	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
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

void SetupLightingDX9(void)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	// `�}�ҥ����\��`
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// `�}��/����localviewer�\��`
	device->SetRenderState(D3DRS_LOCALVIEWER, g_bLocalViewer ? TRUE:FALSE);
	// `�}��/����autonormalize�\��`
	device->SetRenderState(D3DRS_NORMALIZENORMALS, g_bAutoNormalize ? TRUE:FALSE);
	// `�]�w���ҥ�`
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vGlobal_AmbientLight));
	// `���w�ϥ���O����ƨӷ�, �⥦�]���g��SetMaterial�禡���ӳ]�w.`
	device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

	device->SetRenderState(D3DRS_SPECULARENABLE, g_bSpecularReflection ? TRUE:FALSE);

	// `�]�w������誺�ϥ���O`
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Ambient = *(D3DCOLORVALUE*) &g_vMaterialAmbient;
	mtrl.Diffuse = *(D3DCOLORVALUE*) &g_vMaterialDiffuse;
	mtrl.Specular = *(D3DCOLORVALUE*) &g_vMaterialSpecular;
	mtrl.Emissive = *(D3DCOLORVALUE*) &g_vMaterialEmissive;
	mtrl.Power = g_fMaterialShininess;
	// `�g�ѩI�sSetMaterial�ӳ]�w����`
	device->SetMaterial(&mtrl);
	// `�]�w����`
	for ( int i=0; i<g_iNumLights; i++ )
	{
		if ( g_Lights[i].m_bEnabled )
		{
			// `���}�o����`
			device->LightEnable(i, TRUE);
			// `D3D9�g�ѳ]�wD3DLIGHT9 struct�ӳ]�w�����ݩ�`
			D3DLIGHT9 light;
			ZeroMemory( &light, sizeof(light) );

			switch(g_Lights[i].m_eType)
			{
			case LIGHT_DIRECTIONAL:
				// `�]�w����V��`
				light.Type = D3DLIGHT_DIRECTIONAL;
				light.Direction = *(D3DVECTOR *) &g_Lights[i].m_vDirection;	
				break;
			case LIGHT_POINT:
				// `�]�w���I����`
				light.Type = D3DLIGHT_POINT;
				// `�]�w��m`
				light.Position = *(D3DVECTOR *) &g_Lights[i].m_vPosition;
				// `�H�Z�����I���`
				// 1/(CONSTANT+LINEAR*d+QUADRATIC*d^2)
				// `��������CONSTANT, LINEAR, QUADRATIC��`
				light.Attenuation0 = g_Lights[i].m_vAttenuation[0];
				light.Attenuation1 = g_Lights[i].m_vAttenuation[1];
				light.Attenuation2 = g_Lights[i].m_vAttenuation[2];
				break;
			case LIGHT_SPOT:
				// `�]�w���E���O`
				light.Type = D3DLIGHT_SPOT;
				// `�]�w��m`
				light.Position = *(D3DVECTOR *) &g_Lights[i].m_vPosition;
				// `�]�w��V`
				light.Direction = *(D3DVECTOR *) &g_Lights[i].m_vDirection;	
				// `�H�Z�����I���`
				// 1/(CONSTANT+LINEAR*d+QUADRATIC*d^2)
				// `��������CONSTANT, LINEAR, QUADRATIC��`
				light.Attenuation0 = g_Lights[i].m_vAttenuation[0];
				light.Attenuation1 = g_Lights[i].m_vAttenuation[1];
				light.Attenuation2 = g_Lights[i].m_vAttenuation[2];
				// `���W���@���@�b����`
				light.Phi = FastMath::DegreeToRadian(g_Lights[i].m_fSpotlightCutoff);
				// `Inner Cone������, Inner Cone�����S�����װI��.`
				light.Theta = FastMath::DegreeToRadian(g_Lights[i].m_fSpotLightInnerCone);
				// `���W�����װI����`
				light.Falloff = g_Lights[i].m_fSpotlightExponent;
				break;
			}

			// `������Ambient��`
			light.Ambient = *(D3DCOLORVALUE*) &g_Lights[i].m_vAmbientColor;
			// `������Diffuse��`
			light.Diffuse = *(D3DCOLORVALUE*) &g_Lights[i].m_vDiffuseColor;
			// `������Specular��`
			light.Specular = *(D3DCOLORVALUE*) &g_Lights[i].m_vSpecularColor;
			// `�������Ľd��, �b���⥦�]�w���B�I�ƩҮe�p���̤j��.`
			light.Range = 1000.0f;
			// `��SetLight�ӳ]�w�����ݩ�`
			device->SetLight(i, &light);
		}
		else
		{
			// `�����o����`
			device->LightEnable(i, FALSE);
		}
	}
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �����e��
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
	// �]�w����
	SetupLightingDX9();

	// �]�w��Ʈ榡
	device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
	// �e�X��l
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, g_iNumGridVertices, g_iNumGridTriangles, 
		g_pGridIndices, D3DFMT_INDEX16, g_pGridVertices, sizeof(Vertex_V3N3) );
	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
