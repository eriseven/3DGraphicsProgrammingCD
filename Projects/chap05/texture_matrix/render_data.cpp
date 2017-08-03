#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// controller object
CGutUserControl g_Control;
// �K�Ϯy���ഫ�x�}
Matrix4x4 g_texture_matrix;

// �x�Ϊ�4�ӳ��I
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}}
};

int g_iMode = 1;