#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

// mirror vertex type
struct Vertex_V
{
	Vector4 m_Position; // ���I��m
};
// model object
extern CGutModel g_Model;
// controller object
extern CGutUserControl g_Control;
// camer fov
extern float g_fFOV;
// mirror position
extern float g_mirror_z;
// �x��
extern Vertex_V g_Quad[4];
// ���Y��m
extern Vector4 g_eye; 
