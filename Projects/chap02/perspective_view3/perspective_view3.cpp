#include <stdio.h>
#include "Gut.h"
#include "Vector4.h"
#include <d3dx9.h>
#include <GL/glu.h>

void RenderFrameDX9(void);
void RenderFrameOpenGL(void);

void main(int argc, char *argv[])
{
	// ���w�ϥ�DirectX 9��ø��
	char *device = "dx9";

	if ( argc > 1 )
	{
		// �p�G�R�O�C�Ѽƫ��w��OpenGL, �N���OpenGL.
		if ( stricmp(argv[1], "opengl")==0 )
		{
			device = "opengl";
		}
	}

	// �b(100,100)����m�}�Ҥ@�Ӥj�p��(512x512)������
	GutCreateWindow(100, 100, 512, 512, device);

	// ��OpenGL��DirectX��l��
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	// �D�j��
	while( GutProcessMessage() )
	{
		if ( !strcmp(device, "dx9") )
		{
			RenderFrameDX9();
		}
		else
		{
			RenderFrameOpenGL();
		}
	}

	// ����OpenGL/DirectXø�ϸ˸m
	GutReleaseGraphicsDevice();
}

// ���r��Ϊ�8����u
Vector4 g_vertices[] = 
{
	Vector4(-1.0f, 1.0f,-1.0f),
	Vector4(-1.0f,-1.0f,-1.0f),

	Vector4(-1.0f,-1.0f,-1.0f),
	Vector4( 1.0f,-1.0f,-1.0f),

	Vector4( 1.0f,-1.0f,-1.0f),
	Vector4( 1.0f, 1.0f,-1.0f),

	Vector4( 1.0f, 1.0f,-1.0f),
	Vector4(-1.0f, 1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4(-1.0f, 1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4(-1.0f,-1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4( 1.0f,-1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4( 1.0f, 1.0f,-1.0f),
};

// ���Y��m
Vector4 g_eye(0.0f,3.0f,3.0f); 
// ���Y��Ǫ��I
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// ���Y���W�誺��V
Vector4 g_up(0.0f, -1.0f, 1.0f); 

// `�ϥ�DirectX9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
    device->SetRenderState( D3DRS_LIGHTING, FALSE );
    device->Clear(
		0, NULL, // `�M����ӵe��` 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // `�M���C���Z Buffer` 
		D3DCOLOR_ARGB(0, 0, 0, 0), // `�]�w�n���C��M���¦�`
		1.0f, // `�]�w�n��Z�ȲM��1, �]�N�O�����Y�̻�.`
		0 // `�]�w�n��Stencil buffer�M��0, �b�o�S�t.`
	);
	
	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	D3DXVECTOR3 eye(0.0f, 3.0f, 3.0f);
	D3DXVECTOR3 lookat(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, -1.0f, 1.0f);
	// `�p�����Y�ഫ�x�}`
	D3DXMATRIX view_matrix;
	D3DXMatrixLookAtRH(&view_matrix, &eye, &lookat, &up);
	// `�]�w���Y�ഫ�x�}`
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix); 	

	// `�p��D�����v�x�}`
	D3DXMATRIX perspective_matrix;
	D3DXMatrixPerspectiveFovRH(&perspective_matrix, FastMath::DegreeToRadian(90.0f), 1.0f, 1.0f, 100.0f);
	// `�]�w�����ഫ�x�}`
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&perspective_matrix); 	

	device->BeginScene(); // `�}�l�Uø�ϫ��O`
	device->SetFVF(D3DFVF_XYZ); // `�]�w��Ʈ榡`
	// `�e�X���r��8����u`
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, g_vertices, sizeof(Vector4)); 
	device->EndScene(); // `�ŧi�Ҧ���ø�ϫ��O���U���F`
	
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `�p��ó]�w���Y�ഫ�x�}`
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		g_eye[0], g_eye[1], g_eye[2],
		g_lookat[0], g_lookat[1], g_lookat[2],
		g_up[0], g_up[1], g_up[2]);
	// `�p��ó]�w�����ഫ�x�}`
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, 1.0f, 1.0f, 100.0f);

	// `�e�X���r��8����u`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	glDrawArrays(GL_LINES, 0, 16);

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
