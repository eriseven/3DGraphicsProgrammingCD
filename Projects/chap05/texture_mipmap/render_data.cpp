#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// �x�Ϊ�4�ӳ��I
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{{-2.0f, 0.0f, -2.0f}, {0.0f, 4.0f}},
	{{ 2.0f, 0.0f, -2.0f}, {4.0f, 4.0f}},
	{{-2.0f, 0.0f, 2.0f}, {0.0f, 0.0f}},
	{{ 2.0f, 0.0f, 2.0f}, {4.0f, 0.0f}}
};

int g_iFilterMode = 2;

// controller object
CGutUserControl g_Control;
