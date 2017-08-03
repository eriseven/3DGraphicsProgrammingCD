#include "Gut.h"
#include "render_data.h"
#include <d3dx9.h>
#include <d3dx9math.h>

bool InitResourceDX9(void)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �]�w�����ഫ�x�}
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// ���oDirect3D 9�˸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DPRESENT_PARAMETERS d3dpresent;
    
	ZeroMemory( &d3dpresent, sizeof(d3dpresent) );
    d3dpresent.Windowed = TRUE; // �ϥε����Ҧ�
    d3dpresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpresent.BackBufferFormat = D3DFMT_UNKNOWN; // �ϥε����Ҧ��i�H���h�]�w
	d3dpresent.BackBufferCount = 1; // ���Ѥ@��backbuffer
	d3dpresent.EnableAutoDepthStencil = TRUE; // �۰ʶ}��DepthStencil Buffer
	d3dpresent.AutoDepthStencilFormat = D3DFMT_D24S8; // DepthStencil Buffer�Ҧ�
	
	device->Reset(&d3dpresent);

	// ��v�x�}, ���]�����򫫪���V������.
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// ��������
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ���ܤT���Υ��������V
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

// �ϥ�DirectX 9��ø��
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// �����e��
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 50, 50, 50), 1.0f, 0);
	
	// �}�l�Uø�ϫ��O
	device->BeginScene(); 

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// �]�w��Ʈ榡
	// D3DFVF_XYZ = �ϥ�3�ӯB�I�ƨӰO����m
	// D3DFVF_DIFFUSE = �ϥ�32bits��ƫ��A�ӰO��BGRA�C��
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	device->SetRenderState(D3DRS_ZWRITEENABLE, g_bZWrite ? TRUE : FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	switch(g_iBlendMode)
	{
	case 0:
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	case 1:
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}

	Matrix4x4 world_view_matrix;

	for ( int i=0; i<2; i++ )
	{
		switch( g_iOrder[i] )
		{
		case 0:
			// �Ӷ�
			world_view_matrix = g_sun_matrix * g_rot_matrix;
			device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_view_matrix);
			device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
			g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
			break;
		case 1:
			// �a�y
			world_view_matrix = g_earth_matrix * g_rot_matrix;
			device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_view_matrix);
			device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
			g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
			break;
		}
	}

	// �ŧi�Ҧ���ø�ϫ��O���U���F
	device->EndScene(); 
	
	// ��I��backbuffer���e���e�{�X��
    device->Present( NULL, NULL, NULL, NULL );
}
