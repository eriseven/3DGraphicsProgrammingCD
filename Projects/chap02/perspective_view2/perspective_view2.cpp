#include <stdio.h>
#include "Gut.h"
#include "Vector4.h"

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
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `�]�w���Y�ഫ�x�}`
	// `�]���O����ƦC��k�ۦP, �i�H������Matrix4x4�૬��D3DMATRIX.`
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// `�p��X�@�ӨϥΫD�����v���x�}`
	Matrix4x4 perspective_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 1.0f, 100.0f);
	// `�]�w�����ഫ�x�}`
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&perspective_matrix);

	// `�}�l�Uø�ϫ��O`
	device->BeginScene();	
	// `�]�w��Ʈ榡`
	device->SetFVF(D3DFVF_XYZ);
	// `�e�X���r��8����u`
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, g_vertices, sizeof(Vector4)); 
	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	
	// `�e�{�X�I��backbuffer���e��`
	device->Present( NULL, NULL, NULL, NULL );
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `�]�w���Y�ഫ�x�}`
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float *) &view_matrix);
	// `�p��X�@�ӫD�����v���z���x�}`
	Matrix4x4 perspective_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
	// `�]�w�����ഫ�x�}`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float *) &perspective_matrix);

	// `�e�X���r��8����u`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	glDrawArrays(GL_LINES, 0, 16);

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
