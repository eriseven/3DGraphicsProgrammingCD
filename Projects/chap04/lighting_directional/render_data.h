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
extern Vertex_VCN g_Quad[4];
extern Vertex_DX9 g_Quad_dx9[4];

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
extern Vector4 g_vLightColor;

// �p�⥴�����禡
void CalculateAmbientLight(Vertex_VCN *pVertices, int num_vertices);
void CalculateDirectionalLight(Vertex_VCN *pVertices, int num_vertices);

void ConvertToDX9(Vertex_VCN g_quad_in[], Vertex_DX9 g_quad_out[], int num_vertices);
