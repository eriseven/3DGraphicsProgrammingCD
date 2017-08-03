#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// ���Y��m
Vector4 g_eye(0.0f, 0.0f, 2.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// ���Y�ഫ�x�}
Matrix4x4 g_view_matrix;
// �������x�}
Matrix4x4 g_world_matrix;
// �x�Ϊ�4�ӳ��I
// Position
Vertex_V g_Quad[4] =
{
	{-2.0f, -2.0f, 0.0f},
	{ 2.0f, -2.0f, 0.0f},
	{-2.0f,  2.0f, 0.0f},
	{ 2.0f,  2.0f, 0.0f}
};

int g_iFilterMode = 2;