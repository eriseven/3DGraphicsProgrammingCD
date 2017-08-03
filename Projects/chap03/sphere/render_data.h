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

// �y�����
extern Vertex_VC *g_pSphereVertices;
extern unsigned short *g_pSphereIndices;

extern int g_iNumSphereVertices;
extern int g_iNumSphereTriangles;
extern int g_iNumSphereIndices;

// ���Y��m
extern Vector4 g_eye; 
// ���Y��Ǫ��I
extern Vector4 g_lookat; 
// ���Y���W�誺��V
extern Vector4 g_up; 
// ���Y�ഫ�x�}
extern Matrix4x4 g_view_matrix;

// ���Ͳy�ҫ����{��
bool CreateSphere(float radius, Vertex_VC **ppVertices, unsigned short **ppIndices, float *color=NULL, int stacks = 20, int pieces = 20);
