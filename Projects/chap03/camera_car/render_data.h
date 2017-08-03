#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VC
{
	float m_Position[3];
	// R,G,B,A in OpenGL
	// B,G,R,A in Direct3D 9
	// Direct3D 10�i�O�O��ب䤤��1, �ݳ]�w�өw.
	unsigned char m_RGBA[4];
};

// ���r��Ϊ�8����u
extern Vertex_VC g_pyramid_vertices[5];
// 
extern unsigned short g_pyramid_trianglefan_indices[6];

extern unsigned short g_pyramid_trianglelist_indices[12];

extern Vertex_VC g_road_vertices[4];

extern unsigned short g_road_trianglestrip_indices[4];

// �a��
extern char g_map[8][8];


// ���Y��m
extern Vector4 g_eye; 
// ���Y��Ǫ��I
extern Vector4 g_lookat; 
// ���Y���W�誺��V
extern Vector4 g_up; 

//
extern Matrix4x4 g_object_matrix;