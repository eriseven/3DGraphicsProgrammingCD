#include "Vector4.h"
#include "render_data.h"
#include "Gut.h"
#include "GutUserControl.h"

// Ū���ҫ�������
CGutModel g_Model;
// controller object
CGutUserControl g_Control;

float g_fNear = 0.1f;
float g_fFar = 500.0f;

sGutLight g_Lights[NUM_LIGHTS];
