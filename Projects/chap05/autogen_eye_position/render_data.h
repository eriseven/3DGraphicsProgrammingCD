#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

struct Vertex
{
	float m_Position[3]; // ���I��m
};

// �`�Ʃw�q
const float g_fFovW = 45.0f;
// Vertex
extern Vertex g_Quad[4];
// controller object
extern CGutUserControl g_Control;
