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

// `���r��Ϊ�8����u`
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

// `���Y��m`
Vector4 g_eye(0.0f,0.0f,5.0f); 
// `���Y��Ǫ��I`
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// `���Y���W�誺��V`
Vector4 g_up(0.0f, 1.0f,0.0f); 

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
	// `�p��X�@�ӨϥΥ����v���x�}`
	Matrix4x4 orthogonal_matrix = GutMatrixOrthoRH_DirectX(5.0f, 5.0f, 0.1f, 100.0f);
	// `��o��ӯx�}�ۭ�`
	Matrix4x4 view_orthogonal_matrix = view_matrix * orthogonal_matrix;
	// `��Ŷ������y���I�ഫ��ù��y�Шt�W`
	Vector4 vertices[16];
	for ( int i=0; i<16; i++ )
	{
		vertices[i] = g_vertices[i] * view_orthogonal_matrix;
		vertices[i] /= vertices[i].GetW();
	}

	device->BeginScene(); // `�}�l�Uø�ϫ��O`
	device->SetFVF(D3DFVF_XYZ); // `�]�w��Ʈ榡`
	// `�e�X���r��8����u`
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, vertices, sizeof(Vector4)); 
	device->EndScene(); // `�ŧi�Ҧ���ø�ϫ��O���U���F`
	
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}

// `�ϥ�OpenGL��ø��`
void RenderFrameOpenGL(void)
{
	// `�M���e��`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `�p��X�@�ӨϥΥ����v���x�}`
	Matrix4x4 orthogonal_matrix = GutMatrixOrthoRH_OpenGL(5.0f, 5.0f, 0.1f, 100.0f);
	// `��o��ӯx�}�ۭ�`
	Matrix4x4 view_orthogonal_matrix = view_matrix * orthogonal_matrix;
	// `��Ŷ������y���I�ഫ��ù��y�Шt�W`
	Vector4 vertices[16];
	for ( int i=0; i<16; i++ )
	{
		vertices[i] = g_vertices[i] * view_orthogonal_matrix;
		vertices[i] /= vertices[i].GetW();
	}

	// `�e�X���r��8����u`
	glBegin(GL_LINES);
	for ( int l=0; l<8; l++ )
	{
		glVertex3fv((float *) &vertices[l*2]);
		glVertex3fv((float *) &vertices[l*2+1]);
	}
	glEnd();

	// `��I��backbuffer���e���e�{�X��`
	GutSwapBuffersOpenGL();
}
