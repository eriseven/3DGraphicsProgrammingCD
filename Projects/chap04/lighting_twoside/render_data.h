#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VN
{
	float m_Position[3];	// ���I��m
	float m_Normal[3];		// ���I���V
};

// �`�Ʃw�q
const float g_fFovW = 45.0f;
extern Vertex_VN g_Quad[4];

// ���Y��m
extern Vector4 g_eye; 
// ���Y��Ǫ��I
extern Vector4 g_lookat; 
// ���Y���W�誺��V
extern Vector4 g_up; 
// ���Y�ഫ�x�}
extern Matrix4x4 g_view_matrix;
// �������x�}
extern Matrix4x4 g_world_matrix;
// �������
extern Vector4 g_vGlobal_AmbientLight;
extern Vector4 g_vLightAmbient;
extern Vector4 g_vLightDirection;
extern Vector4 g_vLightDiffuse;
extern Vector4 g_vLightSpecular;
// ��������
extern Vector4 g_vMaterialAmbient;
extern Vector4 g_vMaterialDiffuse;
extern Vector4 g_vMaterialSpecular;
extern Vector4 g_vMaterialEmissive;
extern float   g_fMaterialShininess;
// �I������
extern Vector4 g_vMaterialAmbient_Back;
extern Vector4 g_vMaterialDiffuse_Back;
extern Vector4 g_vMaterialSpecular_Back;
extern Vector4 g_vMaterialEmissive_Back;
extern float   g_fMaterialShininess_Back;
