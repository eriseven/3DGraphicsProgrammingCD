#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"
#include "GutModel.h"

// ���Y��m
extern Vector4 g_eye; 
// ���Y��Ǫ��I
extern Vector4 g_lookat; 
// ���Y���W�誺��V
extern Vector4 g_up; 
// ���Y�ഫ�x�}
extern Matrix4x4 g_view_matrix;
extern Matrix4x4 g_world_matrix;
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
//
extern CGutModel g_sun_model, g_earth_model;
//
extern CGutUserControl g_Control;