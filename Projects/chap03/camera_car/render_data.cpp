#include "Vector4.h"
#include "render_data.h"

// ���r��Ϊ�5�ӳ��I��m���C�⪺���
Vertex_VC g_pyramid_vertices[5] = 
{
	{
		{ 0.0f, 1.0f, 0.0f}, // top
		{255, 255, 0, 255},
	},
	{
		{-0.5f, 0.0f, 0.5f},
		{255, 0, 0, 255},
	},
	{
		{ 0.5f, 0.0f, 0.5f},
		{255, 0, 0, 255},
	},
	{
		{ 0.5f, 0.0f, -0.5f},
		{255, 0, 0, 255},
	},
	{
		{-0.5f, 0.0f, -0.5f},
		{255, 0, 0, 255},
	}
};

// �զ����r�𰼭���4�ӤT���Ϊ����I���ޭ�
unsigned short g_pyramid_trianglefan_indices[6] =
{
	0, 
	1, 2, // triangle 1�@
	3,	  // triangle 2
	4,	  // triangle 3
	1	  // triangle 4
};

// �զ����r��6�ӤT���Ϊ����I���ޭ�
unsigned short g_pyramid_trianglelist_indices[12] =
{
	0, 1, 2, // triangle 1�@
	0, 2, 3, // triangle 2
	0, 3, 4, // triangle 3
	0, 4, 1, // triangle 4
};


Vertex_VC g_road_vertices[4] =
{
	{
		{-0.5f, 0.0f, 0.5f},
		{255, 128, 0, 255},
	},
	{
		{ 0.5f, 0.0f, 0.5f},
		{255, 128, 0, 255},
	},
	{
		{ 0.5f, 0.0f,-0.5f},
		{255, 128, 0, 255},
	},
	{
		{-0.5f, 0.0f,-0.5f},
		{255, 128, 0, 255},
	}
};

unsigned short g_road_trianglestrip_indices[4] =
{
	0, 1, 3, 2
};


// ���Y��m
Vector4 g_eye(0.0f, 0.5f, 10.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.5f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 1.0f, 0.0f); 

Matrix4x4 g_object_matrix;

char g_map[8][8] = 
{
	{4, 0, 3, 0, 2, 0, 4, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{5, 0, 1, 0, 6, 0, 3, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{3, 0, 4, 0, 3, 0, 6, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{2, 0, 3, 0, 5, 0, 5, 0},
	{0, 0, 0, 0, 0, 0, 0, 0}
};
