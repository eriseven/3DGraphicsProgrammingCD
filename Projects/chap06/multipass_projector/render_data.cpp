#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// ���Y��m
Vector4 g_eye(0.0f, 0.0f, 5.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// ���Y�ഫ�x�}
Matrix4x4 g_view_matrix;
// �������x�}
Matrix4x4 g_world_matrix;
// �x�Ϊ�4�ӳ��I
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}}
};

Vertex_VT g_FullScreenQuad[4] =
{
	{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 1.0f}, {1.0f, 0.0f}}
};

int g_iMode = 1;

Light g_Light;
Vector4 g_vAmbientLight(0.3f, 0.3f, 0.3f, 1.0f);

// Ū���ҫ�������
CGutModel g_Model;

bool g_bPass0 = true;
bool g_bPass1 = true;
