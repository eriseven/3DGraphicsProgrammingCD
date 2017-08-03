#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// ���Y��m
Vector4 g_eye(0.0f, 0.0f, 2.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// ���Y�ഫ�x�}
Matrix4x4 g_view_matrix;
// �������x�}
Matrix4x4 g_world_matrix;

Vector4 g_vLightAmbient(0.0f, 0.0f, 0.0f, 0.0f);
Vector4 g_vLightDirection(0.0f, 0.0f, 1.0f);
Vector4 g_vLightPosition(0.0f, 0.0f, 0.5f);
Vector4 g_vLightColor(1.0f, 1.0f, 1.0f);
Vector4 g_vLightAttenuation(1.0f, 0.0f, 0.0f);
float	g_fSpotLightCutoff = 30.0f;
float	g_fSpotLightExponent = 1.0f;

int g_iNumGridVertices = 0;
int g_iNumGridIndices = 0;
int g_iNumGridTriangles = 0;
Vertex_VCN *g_pGridVertices = NULL;
Vertex_DX9 *g_pGridVerticesDX9 = NULL;
unsigned short *g_pGridIndices = NULL;

void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		g_quad_in[i].m_Position.StoreXYZ(g_quad_out[i].m_Position);
		GutConvertColor(g_quad_in[i].m_Color, g_quad_out[i].m_Color);
	}
}

// `����x_grids * y_grids�Ӯ�l���ѽL��ҫ�`
bool GenerateGrids(int x_grids, int y_grids, Vertex_VCN **ppVertices, int &num_vertices, unsigned short **ppIndices, int &num_indices, int &num_triangles)
{
	const int triangles_per_row = x_grids * 2;
	const int indices_per_row = triangles_per_row + 2;

	num_vertices = (x_grids + 1) * (y_grids + 1);
	Vertex_VCN *pVertices = (Vertex_VCN *) GutAllocate16BytesAlignedMemory(sizeof(Vertex_VCN)*num_vertices);
	*ppVertices = pVertices;
	if ( pVertices==NULL )
		return false;

	num_indices = indices_per_row * y_grids;
	unsigned short *pIndices = new unsigned short[num_indices];
	*ppIndices = pIndices;
	if ( pIndices==NULL )
	{
		GutRelease16BytesAlignedMemory(pVertices);
		return false;
	}
	//` �ϥ�triangle strip��, �T���μƥإû�������ޭȼƥش�h2`
	num_triangles = num_indices-2;

	Vector4 vCorner(-0.5f, 0.5f, 0.0f, 1.0f);
	Vector4 vStep(1.0f/float(x_grids), -1.0f/float(y_grids), 0.0f, 0.0f);
	Vector4 vPosition = vCorner;
	Vector4 vNormal(0.0f, 0.0f, 1.0f);
	Vector4 vColor(1.0f, 1.0f, 1.0f, 1.0f);

	int x,y;
	int vertex_index = 0;

	for ( y=0; y<=y_grids; y++)
	{
		vPosition[0] = vCorner[0];
		for ( x=0; x<=x_grids; x++, vertex_index++)
		{
			pVertices[vertex_index].m_Position = vPosition;
			pVertices[vertex_index].m_Normal = vNormal;
			pVertices[vertex_index].m_Color = vColor;
			vPosition[0] += vStep[0];
		}
		vPosition[1] += vStep[1];
	}

	const int vertices_per_row = x_grids + 1;
	bool from_left_to_right = true;
	int  index_index = 0;

	for ( y=0; y<y_grids; y++ )
	{
		if ( from_left_to_right )
		{
			// `�b�_�ƦC���ɭ�, �T���αq���ƨ�k.`
			pIndices[index_index++] = y * vertices_per_row;
			pIndices[index_index++] = y * vertices_per_row + vertices_per_row;

			for ( x=0; x<x_grids; x++ )
			{
				vertex_index = y * vertices_per_row + x;
				pIndices[index_index++] = vertex_index + 1;
				pIndices[index_index++] = vertex_index + 1 + vertices_per_row;
			}
		}
		else
		{
			// `�b���ƦC���ɭ�, �T���αq�k�ƨ쥪.`
			pIndices[index_index++] = y * vertices_per_row + x_grids;
			pIndices[index_index++] = (y+1) * vertices_per_row + x_grids;

			for ( x=x_grids; x>0; x-- )
			{
				vertex_index = y * vertices_per_row + x;
				pIndices[index_index++] = vertex_index - 1;
				pIndices[index_index++] = vertex_index - 1 + vertices_per_row;
			}
		}
		from_left_to_right = !from_left_to_right;
	}

	return true;
}

bool ReleaseGridsResource(Vertex_VCN **ppVertices, unsigned short **ppIndices)
{
	if ( *ppVertices )
	{
		GutRelease16BytesAlignedMemory(*ppVertices);
		*ppVertices = NULL;
	}

	if ( *ppIndices )
	{
		delete [] *ppIndices;
		*ppIndices = NULL;
	}

	return true;
}

// `���C���ȳ]�w�����ҥ�`
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		pVertices[i].m_Color = g_vLightAmbient;
	}
}

// `�p���V��, ���u���I���V�M������V����.`
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		// `�D�X�ഫ��b�@�ɮy�Шt�����I���V, RotateVector�禡�u������, �����첾.`
		Vector4 normal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `���I���V����u��V���樤, �M�w�Ϯg�����j��.`
		Vector4 intensity = Vector3Dot(normal, g_vLightDirection);
		// `��intensity�����b�û��j��0���d��`
		intensity.Clamp_to_0();
		// `�֥[�W�p��X�Ӥ�V�����j��`
		pVertices[i].m_Color += intensity * g_vLightColor;
		pVertices[i].m_Color.Clamp_to_1();
	}
}

// `�p���I����, ���M���I��m, ���I���V, ������m����.`
void CalculatePointLight(Vertex_VCN *pVertices, int num_vertices)
{
	for ( int i=0; i<num_vertices; i++ )
	{
		// `�D�X�ഫ��b�@�ɮy�Шt�����I��m`
		Vector4 vPosition = pVertices[i].m_Position * g_world_matrix;
		// `�D�X�ഫ��b�@�ɮy�Шt�����I���V, RotateVector�禡�u������, �����첾.`
		Vector4 vNormal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `�p��X���I��m���������V�����`
		Vector4 vVertex_to_Light = g_vLightPosition - vPosition; 
		float light_distance = vVertex_to_Light.NormalizeAndGetLength();
		// `vDistance�Ψӭp����u�H�Z���I�����1/(a*1 + b*d + c*d^2)������(1, d, d^2)`
		Vector4 vDistance(1.0f, light_distance, light_distance * light_distance);
		// `g_vLightAttenuation�ذO���F�p��I���1/(a + b*d + c*d^2)�ت�(a,b,c)`
		// `Vector3Dot(vDistance, g_vLightAttenuation) = (a,b,c) dot (1,d,d^2) = (a + b*d + c*d^2)`
		Vector4 vAttenuation = Vector3Dot(vDistance, g_vLightAttenuation);
		// `���I���V����u��V���樤, �M�w�Ϯg�����j��.`
		Vector4 vIntensity = Vector3Dot(vNormal, vVertex_to_Light);
		// `��intensity�����b�û��j��0���d��`
		vIntensity.Clamp_to_0();
		// `�֥[�W�H�Z���I����u�j��`
		pVertices[i].m_Color += vIntensity * g_vLightColor / vAttenuation;
		pVertices[i].m_Color.Clamp_to_1();
	}
}

// `�p���I����, ���M���I��m, ���I���V, ������m, ������V, ���W�樤����.`
void CalculateSpotLight(Vertex_VCN *pVertices, int num_vertices)
{
	float fSpotLightCutoffCos = FastMath::Cos( FastMath::DegreeToRadian(g_fSpotLightCutoff) );
	
	for ( int i=0; i<num_vertices; i++ )
	{
		// `�D�X�ഫ��b�@�ɮy�Шt�����I��m`
		Vector4 vPosition = pVertices[i].m_Position * g_world_matrix;
		// `�D�X�ഫ��b�@�ɮy�Шt�����I���V, RotateVector�禡�u������, �����첾.`
		Vector4 vNormal = g_world_matrix.RotateVector(pVertices[i].m_Normal);
		// `�p��X���I��m���������V�����`
		Vector4 vVertex_to_Light = g_vLightPosition - vPosition; 
		float light_distance = vVertex_to_Light.NormalizeAndGetLength();
		// `���I���V����u��V���樤, �i�H�M�w�Ϯg�����j��.`
		Vector4 vCosine = Vector3Dot(g_vLightDirection, vVertex_to_Light);
		// `��vCosine�����b�û��j��0���d��`
		vCosine.Clamp_to_0();
		float fCosine = vCosine.GetX();
		if ( fCosine >= fSpotLightCutoffCos )
		{
			// `���I����u���樤�p��fSpotightCutoffCos��, �~���b���W�d��.`
			Vector4 vDistance(1.0f, light_distance, light_distance * light_distance);
			// `g_vLightAttenuation�ذO���F�p��I���1/(a + b*d + c*d^2)�ت�(a,b,c)`
			// `Vector3Dot(vDistance, g_vLightAttenuation) = (a,b,c) dot (1,d,d^2) = (a + b*d + c*d^2)`
			Vector4 vAttenuation = Vector3Dot(vDistance, g_vLightAttenuation);
			// `����a����W�~�򳡤������I, ���u�|�I��.`
			float fFalloff = pow(fCosine, g_fSpotLightExponent);
			Vector4 vIntensity = Vector3Dot(vNormal, vVertex_to_Light);
			pVertices[i].m_Color += fFalloff * vIntensity * g_vLightColor / vAttenuation;
			pVertices[i].m_Color.Clamp_to_1();
		}
	}
}
