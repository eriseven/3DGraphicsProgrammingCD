#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "render_data.h"
#include <d3dx10.h>

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pSunVertexBuffer		= NULL;
static ID3D10Buffer			*g_pEarthVertexBuffer	= NULL;
static ID3D10Buffer			*g_pMoonVertexBuffer	= NULL;

static ID3D10Buffer			*g_pSphereIndexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static D3DXMATRIX g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// ���JVertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/vertex_color_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// ���JPixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/vertex_color_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

    // �]�wVertex��Ʈ榡
    D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

    if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	D3D10_BUFFER_DESC cbDesc;
	// sun vertex buffer
    cbDesc.ByteWidth = sizeof(Vertex_VC) * g_iNumSphereVertices;
    cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
    cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    cbDesc.CPUAccessFlags = 0;
    cbDesc.MiscFlags = 0;
	// �}��Vertex Buffer�ɦP�ɧ��ƫ����L�h
	D3D10_SUBRESOURCE_DATA sbDesc;
	sbDesc.pSysMem = g_pSunVertices;
	// �t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pSunVertexBuffer ) )
		return false;

	// �}��Vertex Buffer�ɦP�ɧ��ƫ����L�h
	sbDesc.pSysMem = g_pEarthVertices;
	// �t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pEarthVertexBuffer ) )
		return false;

	// �}��Vertex Buffer�ɦP�ɧ��ƫ����L�h
	sbDesc.pSysMem = g_pMoonVertices;
	// �t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pMoonVertexBuffer ) )
		return false;

	// �]�w�@���i�H�Ψө�Index���O����.
    cbDesc.ByteWidth = sizeof(unsigned short) * g_iNumSphereIndices;
    cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
    cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    cbDesc.CPUAccessFlags = 0;
    cbDesc.MiscFlags = 0;
	// �}��Index Buffer�ɦP�ɧ��ƫ����L�h
	sbDesc.pSysMem = g_pSphereIndices;
    // �t�m�@���i�H�s��Index���O����, �]�N�OIndex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &sbDesc, &g_pSphereIndexBuffer ) )
		return false;

	// �t�mShaderŪ���Ѽƪ��O����Ŷ�
    cbDesc.ByteWidth = sizeof(Matrix4x4);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
		return false;
	
	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	D3DXMatrixPerspectiveFovRH(&g_proj_matrix, FastMath::DegreeToRadian(90.0f), 1.0f, 0.1f, 100.0f);

	// �}?rasterizer state����
	D3D10_RASTERIZER_DESC rasterizer_state_desc;
	
	rasterizer_state_desc.FillMode = D3D10_FILL_SOLID;
	rasterizer_state_desc.CullMode = D3D10_CULL_BACK;
	rasterizer_state_desc.FrontCounterClockwise = true;
	rasterizer_state_desc.DepthBias = 0;
	rasterizer_state_desc.DepthBiasClamp = 0.0f;
	rasterizer_state_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_state_desc.DepthClipEnable = false;
	rasterizer_state_desc.ScissorEnable = false;
	rasterizer_state_desc.MultisampleEnable = false;
	rasterizer_state_desc.AntialiasedLineEnable = false;

	if ( D3D_OK != g_pDevice->CreateRasterizerState(&rasterizer_state_desc, &g_pRasterizerState) )
		return false;

	g_pDevice->RSSetState(g_pRasterizerState);

	return true;
}

bool ReleaseResourceDX10(void)
{
 	SAFE_RELEASE(g_pVertexLayout);
 	SAFE_RELEASE(g_pSunVertexBuffer);
 	SAFE_RELEASE(g_pEarthVertexBuffer);
 	SAFE_RELEASE(g_pMoonVertexBuffer);
 	SAFE_RELEASE(g_pSphereIndexBuffer);
 	SAFE_RELEASE(g_pConstantBuffer);
 	SAFE_RELEASE(g_pVertexShader);
 	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	float aspect = (float) height / (float) width;
	D3DXMatrixPerspectiveFovRH(&g_proj_matrix, FastMath::DegreeToRadian(90.0f), aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VC);
	UINT offset = 0;

	// ���o�I�sGutCreateGraphicsDeviceDX10�ɩҲ��ͪ�D3D10����
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer

	// �M���C��
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// �M��Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// �]�wvertex shader
	g_pDevice->VSSetShader(g_pVertexShader);
	// �]�wpixel shader
	g_pDevice->PSSetShader(g_pPixelShader);
	// �]�wvertex shaderŪ���Ѽƪ��O������_
    g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// �]�wvertex��Ʈ榡
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// �]�windex buffer
	g_pDevice->IASetIndexBuffer(g_pSphereIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// �]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	D3DXMATRIX temp_matrix;
	D3DXMATRIX view_matrix, view_proj_matrix, world_view_proj_matrix;
	D3DXMatrixLookAtRH(&view_matrix, (D3DXVECTOR3 *)&g_eye, (D3DXVECTOR3 *)&g_lookat, (D3DXVECTOR3 *)&g_up);
	D3DXMatrixMultiply(&view_proj_matrix, &view_matrix, &g_proj_matrix);

	// �Ӷ�, �a�J�ƹ������౱��
	D3DXMATRIX sun_matrix, sun_ry, sun_rx;
	D3DXMatrixRotationX(&sun_rx, g_fRotate_X);		
	D3DXMatrixRotationY(&sun_ry, g_fRotate_Y);		
	D3DXMatrixMultiply(&sun_matrix, &sun_ry, &sun_rx);
	D3DXMatrixMultiply(&world_view_proj_matrix, &sun_matrix, &view_proj_matrix);
	// �]�wshader�Ѽ�
	D3DXMATRIX *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pSunVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// ���P
	float mercury_rotate_y = 2.0f * MATH_PI * (g_simulation_days / days_a_year_mercury); 

	D3DXMATRIX mercury_matrix, mercury_translate, mercury_rotate;
	D3DXMatrixRotationY(&mercury_rotate, mercury_rotate_y);
	D3DXMatrixTranslation(&mercury_translate, mercury_to_sun_distance, 0.0f, 0.0f);
	D3DXMatrixMultiply(&temp_matrix, &mercury_translate, &mercury_rotate);
	D3DXMatrixMultiply(&mercury_matrix, &temp_matrix, &sun_matrix);
	D3DXMatrixMultiply(&world_view_proj_matrix, &mercury_matrix, &view_proj_matrix);

	// �]�wshader�Ѽ�
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();

	g_pDevice->IASetVertexBuffers(0, 1, &g_pMoonVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// ���P
	float venus_rotate_y = 2.0f * MATH_PI * (g_simulation_days / days_a_year_venus); 

	D3DXMATRIX venus_matrix, venus_rotate, venus_translate;
	D3DXMatrixRotationY(&venus_rotate, venus_rotate_y);
	D3DXMatrixTranslation(&venus_translate, venus_to_sun_distance, 0.0f, 0.0f);
	D3DXMatrixMultiply(&temp_matrix, &venus_translate, &venus_rotate);
	D3DXMatrixMultiply(&venus_matrix, &temp_matrix, &sun_matrix);
	D3DXMatrixMultiply(&world_view_proj_matrix, &venus_matrix, &view_proj_matrix);

	// �]�wshader�Ѽ�
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();

	g_pDevice->IASetVertexBuffers(0, 1, &g_pMoonVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// �a�y
	float earth_rotate_y = 2.0f * MATH_PI * (g_simulation_days / days_a_year); 

	D3DXMATRIX earth_matrix, earth_rotate, earth_translate;
	D3DXMatrixRotationY(&earth_rotate, earth_rotate_y);
	D3DXMatrixTranslation(&earth_translate, earth_to_sun_distance, 0.0f, 0.0f);
	D3DXMatrixMultiply(&temp_matrix, &earth_translate, &earth_rotate);
	D3DXMatrixMultiply(&earth_matrix, &temp_matrix, &sun_matrix);
	D3DXMatrixMultiply(&world_view_proj_matrix, &earth_matrix, &view_proj_matrix);

	// �]�wshader�Ѽ�
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();

	g_pDevice->IASetVertexBuffers(0, 1, &g_pEarthVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// ��G
	float moon_rotate_y = 2.0f * MATH_PI * (g_simulation_days / days_a_month); 

	D3DXMATRIX moon_matrix, moon_rotate, moon_translate;
	D3DXMatrixRotationY(&moon_rotate, moon_rotate_y);
	D3DXMatrixTranslation(&moon_translate, moon_to_earth_distance, 0.0f, 0.0f);
	D3DXMatrixMultiply(&temp_matrix, &moon_translate, &moon_rotate);
	D3DXMatrixMultiply(&moon_matrix, &temp_matrix, &earth_matrix);
	D3DXMatrixMultiply(&world_view_proj_matrix, &moon_matrix, &view_proj_matrix);

	// �]�wshader�Ѽ�
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();

	g_pDevice->IASetVertexBuffers(0, 1, &g_pMoonVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// ���ݵw�鱽����, �M��~��s�e��
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_