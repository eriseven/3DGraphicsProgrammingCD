#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VCN
{
	Vector4 m_Position; // ���I��m
	Vector4 m_Color;	// ���I�C��
	Vector4 m_Normal;	// ���I���V
};

struct Vertex_DX9
{
	float m_Position[3];	// ���I��m
	unsigned char m_Color[4]; // ���I�C��
};

// �`�Ʃw�q
const float g_fFovW = 45.0f;
const int	g_iMaxNumGrids = 100;
const int	g_iMaxNumGridVertices = (g_iMaxNumGrids+1) * (g_iMaxNumGrids+1);
const int	g_iMaxNumGridIndices = g_iMaxNumGrids * (g_iMaxNumGrids * 2 + 2);

// ��l�ҫ����
extern int	g_iNumGridVertices;
extern int	g_iNumGridIndices;
extern int	g_iNumGridTriangles;
extern Vertex_VCN *g_pGridVertices;
extern Vertex_DX9 *g_pGridVerticesDX9;
extern unsigned short *g_pGridIndices;

// ���Y��m
extern Vector4 g_eye; 
// ���Y��Ǫ��I
extern Vector4 g_lookat; 
// ���Y���W�誺��V
extern Vector4 g_up; 
// ���Y�ഫ�x�}
extern Matrix4x4 g_view_matrix;
// �������x�}
extern Matrix4x4 g_world_matrix;
// �������
extern Vector4 g_vLightAmbient;
extern Vector4 g_vLightDirection;
extern Vector4 g_vLightPosition;
extern Vector4 g_vLightColor;
extern Vector4 g_vLightAttenuation;
extern float   g_fSpotLightCutoff;
extern float   g_fSpotLightExponent;

//
void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices);
// ���ͮ�l���禡
bool GenerateGrids(int x_grids, int y_grids, 
				   Vertex_VCN **ppVertices, int &num_vertices, 
				   unsigned short **ppIndices, int &num_indices,
				   int &num_triangles);
// �����l�ҫ��귽
bool ReleaseGridsResource(Vertex_VCN **ppVertices, unsigned short **ppIndices);
// �p�⥴�����禡
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices);
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices);
void CalculatePointLight(Vertex_VCN *pVertices, int num_vertices);
void CalculateSpotLight(Vertex_VCN *pVertices, int num_vertices);
