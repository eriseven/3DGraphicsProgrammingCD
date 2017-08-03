#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"

// �`�Ʃw�q
const float g_fFovW = 45.0f;
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
//
extern CGutModel g_Model;

void CopyVertexBuffer(void *target, sModelVertex *source, int num_vertices);
