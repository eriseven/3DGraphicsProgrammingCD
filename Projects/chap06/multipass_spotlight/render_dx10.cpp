#ifdef _ENABLE_DX10_

#include <d3d10.h>

#include "render_data.h"

#include "Gut.h"
#include "GutTexture_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;
static ID3D10Buffer			*g_pVertexBufferFullScreen = NULL;
static ID3D10Buffer			*g_pConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;

static ID3D10RasterizerState *g_pRasterizerState= NULL;
static ID3D10DepthStencilState	*g_pDepthStencilState_ZDisable = NULL;
static ID3D10BlendState		*g_pBlendState = NULL;

static ID3D10ShaderResourceView *g_pTexture0 = NULL;
static ID3D10ShaderResourceView *g_pTexture1 = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	const char *shader = "../../shaders/texture_dx10.hlsl";
	// ���JVertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL(shader, "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// ���JPixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL(shader, "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

	g_pTexture0 = GutLoadTexture_DX10("../../textures/brickwall.tga");
	g_pTexture1 = GutLoadTexture_DX10("../../textures/spotlight_effect.tga");
	
	if ( g_pTexture0==NULL || g_pTexture1==NULL )
		return false;

    // �]�wVertex��Ʈ榡
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
	// �t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
		return false;

	subDesc.pSysMem = g_FullScreenQuad;
	// �t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBufferFullScreen ) )
		return false;

	// �t�mShader�Ѽƪ��O����Ŷ�
    cbDesc.ByteWidth = sizeof(Matrix4x4);
    cbDesc.Usage = D3D10_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
		return false;
	
	{
		// rasterizer state����
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
	}

	{
		D3D10_BLEND_DESC blend_desc;
		ZeroMemory(&blend_desc, sizeof(blend_desc));

		blend_desc.BlendEnable[0] = TRUE;
		blend_desc.SrcBlend = D3D10_BLEND_ONE;
		blend_desc.DestBlend = D3D10_BLEND_ONE;
		blend_desc.BlendOp = D3D10_BLEND_OP_ADD;
		blend_desc.SrcBlendAlpha = D3D10_BLEND_ONE;
		blend_desc.DestBlendAlpha = D3D10_BLEND_ONE;
		blend_desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;

		blend_desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

		if ( D3D_OK != g_pDevice->CreateBlendState(&blend_desc, &g_pBlendState) )
			return false;

		float dummy[4];
		g_pDevice->OMSetBlendState(g_pBlendState, dummy, 0xffffffff);
	}

	{
		D3D10_DEPTH_STENCIL_DESC depthstencil_desc;
		ZeroMemory(&depthstencil_desc, sizeof(depthstencil_desc));
		
		depthstencil_desc.DepthEnable = TRUE;
		depthstencil_desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthstencil_desc.DepthFunc = D3D10_COMPARISON_ALWAYS;
		depthstencil_desc.StencilEnable = FALSE;

		g_pDevice->CreateDepthStencilState(&depthstencil_desc, &g_pDepthStencilState_ZDisable);
	}

	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	g_proj_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
 	SAFE_RELEASE(g_pVertexLayout);
 	SAFE_RELEASE(g_pVertexBuffer);
 	SAFE_RELEASE(g_pConstantBuffer);
 	SAFE_RELEASE(g_pVertexShader);
 	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);
	SAFE_RELEASE(g_pTexture0);
	SAFE_RELEASE(g_pTexture1);

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
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;
	// ���o�I�sGutCreateGraphicsDeviceDX10�ɩҲ��ͪ�D3D10����
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	// �]�wvertex shaderŪ���Ѽƪ��O������_
    g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// �]�wpixel shaderŪ���Ѽƪ��O������_
    g_pDevice->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// �M�ζK��
	// �]�wvertex shader
	g_pDevice->VSSetShader(g_pVertexShader);
	// �]�wpixel shader
	g_pDevice->PSSetShader(g_pPixelShader);
	// �]�wvertex��Ʈ榡
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// �]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	FLOAT dummy[4];

	{
		g_pDevice->OMSetBlendState(NULL, dummy, 0xffffffff);

		g_pDevice->OMSetDepthStencilState(g_pDepthStencilState_ZDisable, 0);
		// �]�wvertex buffer
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBufferFullScreen, &stride, &offset);
		// �M�β�1�i�K��
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture0);
		// �p��x�}
		Matrix4x4 world_view_proj_matrix; world_view_proj_matrix.Identity();
		// ��sshader�Ѽ�
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = world_view_proj_matrix;
		g_pConstantBuffer->Unmap();
		// �e�X��l
		g_pDevice->Draw(4, 0);
	}

	{
		g_pDevice->OMSetBlendState(g_pBlendState, dummy, 0xffffffff);

		g_pDevice->OMSetDepthStencilState(NULL, 0);
		// �]�wvertex buffer
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

		Vertex_VT *pVertices = NULL;
		g_pVertexBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pVertices );
		memcpy(pVertices, g_Quad, sizeof(g_Quad));
		g_pVertexBuffer->Unmap();

		// �M�β�1�i�K��
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture1);
		// �p��x�}
		Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		Matrix4x4 world_view_proj_matrix = g_world_matrix * view_matrix * g_proj_matrix;
		// ��sshader�Ѽ�
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = world_view_proj_matrix;
		g_pConstantBuffer->Unmap();
		// �e�X��l
		g_pDevice->Draw(4, 0);
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