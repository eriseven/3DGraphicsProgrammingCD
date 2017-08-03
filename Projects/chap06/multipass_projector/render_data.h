#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"

struct Vertex_VT
{
	float m_Position[3]; // ���I��m
	float m_Texcoord[2]; // �K�Ϯy��
};

struct Light
{
	Vector4 m_Position;
	Vector4 m_LookAt;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};

extern Light g_Light;
extern Vector4 g_vAmbientLight;

extern bool g_bPass0;
extern bool g_bPass1;

// �`�Ʃw�q
const float g_fFovW = 45.0f;
// �x��
extern Vertex_VT g_Quad[4];
extern Vertex_VT g_FullScreenQuad[4];
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
//
extern int g_iMode;
//
extern CGutModel g_Model;