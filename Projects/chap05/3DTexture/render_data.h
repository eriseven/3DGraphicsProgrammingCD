#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VT
{
	float m_Position[3]; // ���I��m
	float m_Texcoord[3]; // �K�Ϯy��
};

// �`�Ʃw�q
const float g_fFovW = 45.0f;
// �x��
extern Vertex_VT g_Quad[4];
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
extern Matrix4x4 g_texture_matrix;

