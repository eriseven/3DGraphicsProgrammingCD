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
extern Vertex_VC *g_pSunVertices;
extern Vertex_VC *g_pEarthVertices;
extern Vertex_VC *g_pMoonVertices;
extern unsigned short *g_pSphereIndices;

extern int g_iNumSphereVertices;
extern int g_iNumSphereTriangles;
extern int g_iNumSphereIndices;

extern float g_simulation_days;

extern float g_fRotate_X, g_fRotate_Y;

const float PI = 3.14159;
const float PI_double = PI * 2.0f;
const float days_a_year = 365.0f;
const float days_a_month = 28.0f;
const float days_a_year_mercury = 88.0f;
const float days_a_year_venus = 224.7f;
const float sun_spin_x = 180.0f;
const float earth_to_sun_distance = 8.0f; // �a�y���Ӷ������]��
const float mercury_to_sun_distance = 3.0f; // ���P���Ӷ������]��
const float venus_to_sun_distance = 5.0f; // ���P���Ӷ������]��
const float moon_to_earth_distance = 2.0f; // ��y���a�y�����]��
const float simulation_speed = 60.0f; // 1��۷��60��

// ���Y��m
extern Vector4 g_eye; 
// ���Y��Ǫ��I
extern Vector4 g_lookat; 
// ���Y���W�誺��V
extern Vector4 g_up; 
// ���Y�ഫ�x�}
extern Matrix4x4 g_view_matrix;
// 
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;

// ���Ͳy�ҫ����{��
bool CreateSphere(float radius, Vertex_VC **ppVertices, unsigned short **ppIndices, float *color=NULL, int stacks = 20, int pieces = 20);
