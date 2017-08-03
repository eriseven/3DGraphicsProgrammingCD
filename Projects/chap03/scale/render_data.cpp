#include "Vector4.h"

// ���r��Ϊ�5�ӳ��I
Vector4 g_vertices[5] = 
{
	Vector4(-1.0f, 1.0f, 0.0f),
	Vector4(-1.0f,-1.0f, 0.0f),
	Vector4( 1.0f,-1.0f, 0.0f),
	Vector4( 1.0f, 1.0f, 0.0f),
	Vector4( 0.0f, 0.0f, 1.0f),
};

// �s���X���r��8����u�����ޭ�
unsigned short g_indices[16] =
{
	0, 1,
	1, 2,
	2, 3,
	3, 0,
	0, 4,
	1, 4,
	2, 4,
	3, 4
};

// 4�Ӫ��r�𪺦첾
Vector4 g_position[4] = 
{
	Vector4(-2.0f,-2.0f, 0.0f),
	Vector4( 2.0f,-2.0f, 0.0f),
	Vector4(-2.0f, 2.0f, 0.0f),
	Vector4( 2.0f, 2.0f, 0.0f),
};
// 4�Ӫ��r���Y���
Vector4 g_scale[4] = 
{
	Vector4( 1.0f, 1.0f, 1.0f),
	Vector4( 1.0f, 1.5f, 4.0f),
	Vector4( 1.0f, 1.0f, 2.0f),
	Vector4( 1.5f, 1.5f, 3.0f),
};

// ���Y��m
Vector4 g_eye(0.0f, 8.0f, 1.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 1.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 0.0f, 1.0f); 
