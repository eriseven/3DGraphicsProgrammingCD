#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

// �w�q���I���
struct Vertex_VT
{
	float m_Position[3]; // ���I��m
	float m_Texcoord[2]; // �K�Ϯy��
};
// �`�Ʃw�q
const float g_fFovW = 45.0f;
// �x��
extern Vertex_VT g_Quad[4];
// controller object
extern CGutUserControl g_Control;

extern int g_iFilterMode;
extern bool g_bAnisotropic;