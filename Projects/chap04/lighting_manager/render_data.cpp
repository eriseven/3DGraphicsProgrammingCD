#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

int g_iWindowWidth = 512;
int g_iWindowHeight = 512;

float g_fOrthoSize = 10.0f;
float g_fOrthoWidth = g_fOrthoSize;
float g_fOrthoHeight = g_fOrthoSize;

// ���Y��m
Vector4 g_eye(0.0f, 0.0f, 10.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// ���Y�ഫ�x�}
Matrix4x4 g_view_matrix;
// ���󲾰ʯx�}
Matrix4x4 g_world_matrix;
// �������
Vector4 g_vLightAmbient(0.1f, 0.1f, 0.1f, 0.1f);
Vector4 g_vLightDirection(0.0f, 0.0f, 1.0f, 0.0f);
Vector4 g_vLightColor(1.0f, 1.0f, 1.0f, 1.0f);

Vertex_VCN g_LightPosition[NUM_LIGHTS];
Vertex_DX9 g_LightPosition_dx9[NUM_LIGHTS];

// �x�Ϊ�4�ӳ��I
// Position, Normal
Vertex_VCN g_Quad[4] =
{
	{ Vector4(-0.5f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ Vector4( 0.5f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ Vector4(-0.5f,  0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ Vector4( 0.5f,  0.5f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) }
};

Vertex_DX9 g_Quad_dx9[4];

float g_fMaterialShininess = 30;
Vector4 g_vAmbientLight(0.1f, 0.1f, 0.1f, 0.1f);

int g_iNumActivatedLights = 0;
Light g_ActivatedLights[MAX_ACTIVATED_LIGHTS];

void LightManager(void)
{
	const float fLightRange = 2.0f;

	Vector4 vPosition = g_world_matrix[3];
	g_iNumActivatedLights = 0;

	for ( int i=0; i<NUM_LIGHTS && g_iNumActivatedLights < MAX_ACTIVATED_LIGHTS; i++ )
	{
		Vector4 vDiff = vPosition - g_LightPosition[i].m_Position;
		float fLength = vDiff.Length();
		if ( fLength < fLightRange )
		{
			g_ActivatedLights[g_iNumActivatedLights].m_Position = g_LightPosition[i].m_Position;
			float fAttenuation = 1.0f - fLength/fLightRange;
			g_ActivatedLights[g_iNumActivatedLights].m_Diffuse = g_LightPosition[i].m_Color * fAttenuation;
			g_ActivatedLights[g_iNumActivatedLights].m_Specular = g_LightPosition[i].m_Color * fAttenuation;
			g_iNumActivatedLights++;
		}
	}
}

void ConvertToDX9(Vertex_VCN g_vertices_in[], Vertex_DX9 g_vertices_out[], int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		g_vertices_in[i].m_Position.StoreXYZ(g_vertices_out[i].m_Position);
		g_vertices_in[i].m_Normal.StoreXYZ(g_vertices_out[i].m_Normal);
		GutConvertColor(g_vertices_in[i].m_Color, g_vertices_out[i].m_Color);
	}
}
