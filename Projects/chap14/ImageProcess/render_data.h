#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutUserControl.h"
#include "GutModel.h"

struct Vertex_VT
{
	float m_Position[3]; // ���I��m
	float m_Texcoord[2]; // �K�Ϯy��
};

extern int g_iPosteffect;
extern float g_fSaturation;
// �x��
extern Vertex_VT g_FullScreenQuad[4];
//
extern Matrix4x4 g_SepiaMatrix;
extern Matrix4x4 g_SaturateMatrix;