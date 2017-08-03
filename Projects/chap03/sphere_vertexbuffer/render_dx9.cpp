#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
static LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;

bool InitResourceDX9(void)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `�]�w�����ഫ�x�}`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// `��������`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// `���ܤT���Υ��������V`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	for ( int i=0; i<g_iNumSphereVertices; i++ )
	{
		unsigned char temp = g_pSphereVertices[i].m_RGBA[0];
		g_pSphereVertices[i].m_RGBA[0] = g_pSphereVertices[i].m_RGBA[2];
		g_pSphereVertices[i].m_RGBA[2] = temp;
	}
	
	int vsize = g_iNumSphereVertices * sizeof(Vertex_VC);

	if ( D3D_OK!=device->CreateVertexBuffer(vsize, 0, 0, D3DPOOL_MANAGED, &g_pVertexBuffer, NULL) )
	{
		return false;
	}

	Vertex_VC *pVertexArray = NULL;
	// `���oVertex Buffer���O�����m`
	g_pVertexBuffer->Lock(0, vsize, (void **)&pVertexArray, 0);
	memcpy(pVertexArray, g_pSphereVertices, vsize);
	g_pVertexBuffer->Unlock();

	int isize = g_iNumSphereIndices * sizeof(unsigned short);

	if ( D3D_OK!=device->CreateIndexBuffer(isize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIndexBuffer, NULL) )
	{
		return false;
	}

	unsigned short *pIndexArray = NULL;
	// `���oIndex Buffer���O�����m`
	g_pIndexBuffer->Lock(0, isize, (void **)&pIndexArray, 0);
	memcpy(pIndexArray, g_pSphereIndices, isize);
	g_pIndexBuffer->Unlock();

	return true;
}

bool ReleaseResourceDX9(void)
{
	if ( g_pVertexBuffer )
	{
		g_pVertexBuffer->Release();
		g_pVertexBuffer = NULL;
	}

	if ( g_pIndexBuffer )
	{
		g_pIndexBuffer->Release();
		g_pIndexBuffer = NULL;
	}

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// `���oDirect3D9�˸m`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	GutResetGraphicsDeviceDX9();

	// `�]�w�����ഫ�x�}`
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);

	// `��������`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// `���ܤT���Υ��������V`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

// `�ϥ�Direct3D9��ø��`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	
	// `�}�l�Uø�ϫ��O`
	device->BeginScene(); 

	// `�p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// `�]�w��Ʈ榡`
	// D3DFVF_XYZ = `�ϥ�3�ӯB�I�ƨӰO����m`
	// D3DFVF_DIFFUSE = `�ϥ�32bits��ƫ��A�ӰO��BGRA�C��`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	device->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex_VC));
	device->SetIndices(g_pIndexBuffer);

	// `�e�X�y`
	device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // `���w�ҭn�e���򥻹ϧκ���`
		0, 
		0, // `�|�ϥΪ��̤p���I�s��, �ƹ�W�S�Ӥj�γB.`
		g_iNumSphereVertices, // `���I�}�C�̦��X�ӳ��I`
		0, 
		g_iNumSphereTriangles // `�n�e�X�X�Ӱ򥻹ϧ�`
	);

	// `�ŧi�Ҧ���ø�ϫ��O���U���F`
	device->EndScene(); 
	
	// `��I��backbuffer���e���e�{�X��`
    device->Present( NULL, NULL, NULL, NULL );
}
