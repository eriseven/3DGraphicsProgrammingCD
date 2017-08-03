#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

struct Vertex_VT
{
	Vector4 m_Position; // ���I��m
	Vector4 m_Texcoord; // �K�Ϯy��
};

struct Light
{
	Vector4 m_Position;
	Vector4 m_Direction;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};
// light info
extern Light g_Light;
extern Vector4 g_vAmbientLight;
// controller object
extern CGutUserControl g_Control;
// model object
extern CGutModel g_Model;
// camer afov
extern float g_fFOV;
// mirror position
extern float g_mirror_z;
// �x��
extern Vertex_VT g_Quad[4];
