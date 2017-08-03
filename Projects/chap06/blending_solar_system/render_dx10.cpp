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

static ID3D10BlendState		*g_pBlendState_ADD = NULL;
static ID3D10BlendState		*g_pBlendState_BLEND = NULL;

static ID3D10DepthStencilState *g_pDepthStencil_ZWriteON = NULL;
static ID3D10DepthStencilState *g_pDepthStencil_ZWriteOFF = NULL;

static Matrix4x4 g_proj_matrix;

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
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);

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

	D3D10_BLEND_DESC blend_state_desc;
	ZeroMemory(&blend_state_desc, sizeof(blend_state_desc));
	
	blend_state_desc.AlphaToCoverageEnable = FALSE;
	blend_state_desc.BlendEnable[0] = TRUE;
	blend_state_desc.BlendOp = D3D10_BLEND_OP_ADD;
	blend_state_desc.SrcBlend = D3D10_BLEND_ONE;
	blend_state_desc.DestBlend = D3D10_BLEND_ONE;
	blend_state_desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blend_state_desc.SrcBlendAlpha = D3D10_BLEND_ONE;
	blend_state_desc.DestBlendAlpha = D3D10_BLEND_ONE;
	blend_state_desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	// ADD�V��Ҧ�
	if ( D3D_OK != g_pDevice->CreateBlendState(&blend_state_desc, &g_pBlendState_ADD) )
		return false;

	blend_state_desc.SrcBlend = D3D10_BLEND_SRC_ALPHA ;
	blend_state_desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;

	blend_state_desc.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
	blend_state_desc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;

	// Alpha Blend�V��Ҧ�
	if ( D3D_OK != g_pDevice->CreateBlendState(&blend_state_desc, &g_pBlendState_BLEND) )
		return false;

	D3D10_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
	
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D10_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
	depth_stencil_desc.StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;
	// �}��zbuffer write
	if ( D3D_OK != g_pDevice->CreateDepthStencilState(&depth_stencil_desc, &g_pDepthStencil_ZWriteON) )
		return false;

	depth_stencil_desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;

	// ����zbuffer write
	if ( D3D_OK != g_pDevice->CreateDepthStencilState(&depth_stencil_desc, &g_pDepthStencil_ZWriteOFF) )
		return false;

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

	SAFE_RELEASE(g_pBlendState_BLEND);
	SAFE_RELEASE(g_pBlendState_ADD);

	SAFE_RELEASE(g_pDepthStencil_ZWriteON);
	SAFE_RELEASE(g_pDepthStencil_ZWriteOFF);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.2f, 0.2f, 0.2f, 0.0f);
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

	if ( g_bZWrite )
	{
		g_pDevice->OMSetDepthStencilState(g_pDepthStencil_ZWriteON, 0);
	}
	else
	{
		g_pDevice->OMSetDepthStencilState(g_pDepthStencil_ZWriteOFF, 0);
	}

	float BlendFactor[4] = {0,0,0,0};

	switch(g_iBlendMode)
	{
	case 0:
		g_pDevice->OMSetBlendState(g_pBlendState_BLEND, BlendFactor, 0xffffffff);
		break;
	case 1:
		g_pDevice->OMSetBlendState(g_pBlendState_ADD, BlendFactor, 0xffffffff);
		break;
	default:
		break;
	}

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 view_proj_matrix = view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix;
	Matrix4x4 *pConstData;

	for ( int i=0; i<2; i++ )
	{
		switch( g_iOrder[i] )
		{
		case 0:
			// �Ӷ�, �a�J�ƹ������౱��
			world_view_proj_matrix = g_sun_matrix * g_rot_matrix * view_proj_matrix;
			// �]�wshader�Ѽ�
			g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
			*pConstData = world_view_proj_matrix;
			g_pConstantBuffer->Unmap();
			// 
			g_pDevice->IASetVertexBuffers(0, 1, &g_pSunVertexBuffer, &stride, &offset);
			g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);
			break;

		case 1:
			// �a�y
			world_view_proj_matrix = g_earth_matrix * g_rot_matrix * view_proj_matrix;
			// �]�wshader�Ѽ�
			g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
			*pConstData = world_view_proj_matrix;
			g_pConstantBuffer->Unmap();

			g_pDevice->IASetVertexBuffers(0, 1, &g_pEarthVertexBuffer, &stride, &offset);
			g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);
			break;
		}
	}

	// ���ݵw�鱽����, �M��~��s�e��
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_