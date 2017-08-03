#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "render_data.h"

#include "Gut.h"
#include "GutTexture_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader[2]	= {NULL, NULL};
static ID3D10PixelShader	*g_pPixelShader[2]	= {NULL, NULL};
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10ShaderResourceView *g_pTexture[2] = {NULL, NULL};

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// `���JŪ����i�K�Ϫ�Shader`
	{
		g_pVertexShader[0] = GutLoadVertexShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader[0] )
			return false;
		g_pPixelShader[0] = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "PS", "ps_4_0");
		if ( NULL==g_pPixelShader[0] )
			return false;
	}
	// `���JŪ����i�K�Ϫ�Shader`
	{
		g_pVertexShader[1] = GutLoadVertexShaderDX10_HLSL("../../shaders/textures_add_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader[1] )
			return false;
		g_pPixelShader[1] = GutLoadPixelShaderDX10_HLSL("../../shaders/textures_add_dx10.hlsl", "PS", "ps_4_0");
		if ( NULL==g_pPixelShader[1] )
			return false;
	}
	// `���J��i�K��`
	g_pTexture[0] = GutLoadTexture_DX10("../../textures/brickwall.tga");
	if ( NULL==g_pTexture[0] )
		return false;
	g_pTexture[1] = GutLoadTexture_DX10("../../textures/graffiti_happy.tga");
	if ( NULL==g_pTexture[1] )
		return false;

    // `�]�wVertex��Ʈ榡`
    D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

    if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	D3D10_BUFFER_DESC cbDesc;
	// vertex buffer
    cbDesc.ByteWidth = sizeof(Vertex_VT) * 4;
    cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
    cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA subDesc;
	ZeroMemory(&subDesc, sizeof(subDesc));
	subDesc.pSysMem = g_Quad;
	// `�t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.`
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
		return false;

	// `�t�mShader�Ѽƪ��O����Ŷ�`
    cbDesc.ByteWidth = sizeof(Matrix4x4);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
		return false;
	
	// `�p���v�x�}`
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);

	// `rasterizer state����`
	D3D10_RASTERIZER_DESC rasterizer_state_desc;
	
	rasterizer_state_desc.FillMode = D3D10_FILL_SOLID;
	rasterizer_state_desc.CullMode = D3D10_CULL_NONE;
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
 	SAFE_RELEASE(g_pVertexBuffer);
 	SAFE_RELEASE(g_pConstantBuffer);
 	SAFE_RELEASE(g_pVertexShader[0]);
 	SAFE_RELEASE(g_pVertexShader[1]);
 	SAFE_RELEASE(g_pPixelShader[0]);
 	SAFE_RELEASE(g_pPixelShader[1]);
	SAFE_RELEASE(g_pRasterizerState);
	SAFE_RELEASE(g_pTexture[0]);
	SAFE_RELEASE(g_pTexture[1]);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;
	// `���o�I�sGutCreateGraphicsDeviceDX10�ɩҲ��ͪ�D3D10����`
	// frame buffer
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	// depth/stencil buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	// `�M���C��`
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// `�M��`Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// `�]�wvertex shaderŪ���Ѽƪ��O������_`
    g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// `�M�ζK��`
	switch(g_iMode)
	{
	case 1:
		// `�]�w`vertex shader
		g_pDevice->VSSetShader(g_pVertexShader[0]);
		// `�]�w`pixel shader
		g_pDevice->PSSetShader(g_pPixelShader[0]);
		// `�M�β�1�i�K��`
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture[0]);
		break;
	case 2:
		// `�]�w`vertex shader
		g_pDevice->VSSetShader(g_pVertexShader[0]);
		// `�]�w`pixel shader
		g_pDevice->PSSetShader(g_pPixelShader[0]);
		// `�M�β�2�i�K��`
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture[1]);
		break;
	case 3:
		// `�]�w`vertex shader
		g_pDevice->VSSetShader(g_pVertexShader[1]);
		// `�]�w`pixel shader
		g_pDevice->PSSetShader(g_pPixelShader[1]);
		// `�M�Ψ�i�K��`
		g_pDevice->PSSetShaderResources(0, 2, &g_pTexture[0]);
		break;
	}
	// `�]�wvertex��Ʈ榡`
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// `�]�wvertex buffer`
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// `�]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip`
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// `�p��x�}`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_proj_matrix = object_matrix * view_matrix * g_proj_matrix;
	// `��sshader�Ѽ�`
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// `�e�X��l`
	g_pDevice->Draw(4, 0);
	// `���ݵw�鱽����, �M��~��s�e��.`
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_