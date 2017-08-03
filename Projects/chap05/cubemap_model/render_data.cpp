#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// ���Y��m
Vector4 g_eye(0.0f, 0.0f, 30.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// ���Y�ഫ�x�}
Matrix4x4 g_view_matrix;
// �������x�}
Matrix4x4 g_world_matrix;
// Ū���ҫ�������
CGutModel g_Model;

// �ഫ���I��ƪ��禡, Direct3D��OpenGL�i�H�@��.
void CopyVertexBuffer(void *target, sModelVertex *source, int num_vertices)
{
	// �o�ӽd�Ҥ�, OpenGL��Direct3D����ɥu�ݭnPosition & Normal
	struct sRunTimeVertex
	{
		float m_Position[3];
		float m_Normal[3];
	};

	sRunTimeVertex *pVertexArray = (sRunTimeVertex *) target;

	for ( int v=0; v<num_vertices; v++ )
	{
		sRunTimeVertex *p = pVertexArray + v;

		p->m_Position[0] = source[v].m_Position[0];
		p->m_Position[1] = source[v].m_Position[1];
		p->m_Position[2] = source[v].m_Position[2];

		p->m_Normal[0] = source[v].m_Normal[0];
		p->m_Normal[1] = source[v].m_Normal[1];
		p->m_Normal[2] = source[v].m_Normal[2];
	}
}
