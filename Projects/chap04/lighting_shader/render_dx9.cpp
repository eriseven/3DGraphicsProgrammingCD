#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXSHADER9 g_pDirLight_VS = NULL;
static LPDIRECT3DVERTEXSHADER9 g_pPointLight_VS = NULL;
static LPDIRECT3DVERTEXSHADER9 g_pSpotLight_VS = NULL;
static LPDIRECT3DVERTEXSHADER9 g_pSelected_VS = NULL;

static LPDIRECT3DPIXELSHADER9  g_pVertexColor_PS = NULL;

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_view_proj_matrix;
static Matrix4x4 g_proj_matrix;

bool InitResourceDX9(void)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `���JPixel Shader`
	g_pVertexColor_PS = GutLoadPixelShaderDX9_HLSL("../../shaders/vertex_lighting_directional.shader", "PS", "ps_2_0");
	if ( g_pVertexColor_PS==NULL )
		return false;
	// `���JDirectional Light Vertex Shader`
	g_pDirLight_VS = GutLoadVertexShaderDX9_HLSL("../../shaders/vertex_lighting_directional.shader", "VS", "vs_1_1");
	if ( g_pDirLight_VS==NULL )
		return false;
	// `���JPoint Light Vertex Shader`
	g_pPointLight_VS = GutLoadVertexShaderDX9_HLSL("../../shaders/vertex_lighting_point.shader", "VS", "vs_1_1");
	if ( g_pPointLight_VS==NULL )
		return false;
	// `���JSpot Light Vertex Shader`
	g_pSpotLight_VS = GutLoadVertexShaderDX9_HLSL("../../shaders/vertex_lighting_spot.shader", "VS", "vs_1_1");
	if ( g_pSpotLight_VS==NULL )
		return false;

	// `�����ഫ�x�}`
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);
	// `���Y�ഫ�x�}`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `�ƥ���view_matrix��proj_matrix�ۭ�`
	g_view_proj_matrix = g_view_matrix * g_proj_matrix;

	// `�e�X���V��ϦV���T����`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pVertexColor_PS);
	SAFE_RELEASE(g_pDirLight_VS);
	SAFE_RELEASE(g_pPointLight_VS);
	SAFE_RELEASE(g_pSpotLight_VS);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	GutResetGraphicsDeviceDX9();
	// `��v�x�}, ���]�����򫫪���V������.`
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	g_view_proj_matrix = g_view_matrix * g_proj_matrix;
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�e�X���V��ϦV���T����`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

static void SetupLightingDX9(void)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	int base = 12;
	Vector4 vShininess = g_fMaterialShininess;
	Vector4 vAmbient, vDiffuse, vSpecular, vSpotLightCoeff;
	// `�]�w���ҥ�`
	device->SetVertexShaderConstantF(base, &g_vAmbientLight[0], 1);
	
	// `�]�w����`
	switch(g_iSelectedLight)
	{
	// `��V��`
	default:
	case 0:
		g_pSelected_VS = g_pDirLight_VS;
		
		vAmbient = g_vMaterialAmbient * g_Lights[0].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[0].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[0].m_vSpecularColor;

		device->SetVertexShaderConstantF(base + 1, &g_Lights[0].m_vDirection.x, 1);
		device->SetVertexShaderConstantF(base + 2, &vAmbient.x, 1);
		device->SetVertexShaderConstantF(base + 3, &vDiffuse.x, 1);
		device->SetVertexShaderConstantF(base + 4, &vSpecular.x, 1);
		device->SetVertexShaderConstantF(base + 5, &vShininess.x, 1);

		break;
	// `�I����`
	case 1:
		g_pSelected_VS = g_pPointLight_VS;
		
		vAmbient = g_vMaterialAmbient * g_Lights[1].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[1].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[1].m_vSpecularColor;

		device->SetVertexShaderConstantF(base + 1, &g_Lights[1].m_vPosition.x, 1);
		device->SetVertexShaderConstantF(base + 2, &g_Lights[1].m_vAttenuation.x, 1);
		device->SetVertexShaderConstantF(base + 3, &vAmbient.x, 1);
		device->SetVertexShaderConstantF(base + 4, &vDiffuse.x, 1);
		device->SetVertexShaderConstantF(base + 5, &vSpecular.x, 1);
		device->SetVertexShaderConstantF(base + 6, &vShininess.x, 1);

		break;
	// `�E���O`
	case 2:
		g_pSelected_VS = g_pSpotLight_VS;
		
		vAmbient = g_vMaterialAmbient * g_Lights[2].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[2].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[2].m_vSpecularColor;

		float spotlight_cutoff_cosine = FastMath::Cos( FastMath::DegreeToRadian(g_Lights[2].m_fSpotlightCutoff * 0.5f) );
		vSpotLightCoeff.Set(spotlight_cutoff_cosine, g_Lights[2].m_fSpotlightExponent, 0.0f, 0.0f);

		device->SetVertexShaderConstantF(base + 1, &g_Lights[2].m_vPosition.x, 1);
		device->SetVertexShaderConstantF(base + 2, &g_Lights[2].m_vDirection.x, 1);
		device->SetVertexShaderConstantF(base + 3, &g_Lights[2].m_vAttenuation.x, 1);
		device->SetVertexShaderConstantF(base + 4, &vSpotLightCoeff.x, 1);
		device->SetVertexShaderConstantF(base + 5, &vAmbient.x, 1);
		device->SetVertexShaderConstantF(base + 6, &vDiffuse.x, 1);
		device->SetVertexShaderConstantF(base + 7, &vSpecular.x, 1);
		device->SetVertexShaderConstantF(base + 8, &vShininess.x, 1);

		break;
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
	// `�]�w��Ʈ榡`
	device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
	// `�M��shader`
	device->SetVertexShader(g_pSelected_VS);
	device->SetPixelShader(g_pVertexColor_PS);
	// `�]�w����`
	SetupLightingDX9();
	// `�]�w�ഫ�x�}`
	Matrix4x4 world_view_proj_matrix = g_world_matrix * g_view_proj_matrix;
	device->SetVertexShaderConstantF(0, &world_view_proj_matrix[0][0], 4);
	device->SetVertexShaderConstantF(4, &g_world_matrix[0][0], 4);
	// `���Y��m, �p��Specular�|�Ψ�.`
	device->SetVertexShaderConstantF(8, &g_eye[0], 1);
	// `�e�X��l`
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, g_iNumGridVertices, g_iNumGridTriangles, 
		g_pGridIndices, D3DFMT_INDEX16, g_pGridVertices, sizeof(Vertex_V3N3) );
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
