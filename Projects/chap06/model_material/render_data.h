#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

/*
struct Vertex_VT
{
	float m_Position[3]; // ���I��m
	float m_Texcoord[2]; // �K�Ϯy��
};

struct Light
{
	Vector4 m_Position;
	Vector4 m_Direction;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
	Vector4 m_Ambient;
};
extern Light g_Light0;
extern Light g_Light1;

*/

#define NUM_LIGHTS 2
extern sGutLight g_Lights[NUM_LIGHTS];

extern Vector4 g_vAmbientLight;

// �`�Ʃw�q
const float g_fFovW = 60.0f;
extern float g_fNear;
extern float g_fFar;
// controller object
extern CGutUserControl g_Control;
// model object
extern CGutModel g_Model;