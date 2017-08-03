#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "render_data.h"

#include "Gut.h"
#include "GutTexture_DX10.h"


static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;
static ID3D10Buffer			*g_pSphereIndexBuffer= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// shader loading
	{
		const char *shaders = "../../shaders/cubemap_photovr_dx10.hlsl";
		// ���JVertex Shader
		g_pVertexShader = GutLoadVertexShaderDX10_HLSL(shaders, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader )
			return false;
		// ���JPixel Shader
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL(shaders, "PS", "ps_4_0");
		if ( NULL==g_pPixelShader )
			return false;
	}

	// ���J�K��
	{
		const char *texture_array[] = 
		{
			"../../textures/uffizi_right.tga",
			"../../textures/uffizi_left.tga",
			"../../textures/uffizi_top.tga",
			"../../textures/uffizi_bottom.tga",
			"../../textures/uffizi_back.tga", // `�k��y�Шt�W Z+ �����Y���.`
			"../../textures/uffizi_front.tga" // `�k��y�Шt�W Z- �����Y�e��.`
		};
			
		g_pTexture = GutLoadCubemapTexture_DX10(texture_array);
	}

	{
		// �]�wVertex��Ʈ榡
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	D3D10_BUFFER_DESC cbDesc;
	D3D10_SUBRESOURCE_DATA subDesc;

	{
		// vertex buffer
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		cbDesc.ByteWidth = sizeof(Vertex_VN) * g_iNumSphereVertices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		// �}��Vertex Buffer�ɦP�ɧ��ƫ����L�h
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_pSphereVertices;
		// �t�mVertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
			return false;
	}

	{
		// �]�w�@���i�H�Ψө�Index���O����.
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		cbDesc.ByteWidth = sizeof(unsigned short) * g_iNumSphereIndices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		// �}��Index Buffer�ɦP�ɧ��ƫ����L�h
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_pSphereIndices;
		// �t�mIndex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pSphereIndexBuffer ) )
			return false;
	}

	{
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		// �t�mShader�Ѽƪ��O����Ŷ�
		cbDesc.ByteWidth = sizeof(Matrix4x4);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		// �t�mShader Constant
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
			return false;
	}

	// rasterizer state����
	{
		D3D10_RASTERIZER_DESC rs;
		ZeroMemory(&rs, sizeof(rs));

		rs.FillMode = D3D10_FILL_SOLID;
		rs.CullMode = D3D10_CULL_NONE;
		rs.FrontCounterClockwise = true;
		rs.DepthBias = 0;
		rs.DepthBiasClamp = 0.0f;
		rs.SlopeScaledDepthBias = 0.0f;
		rs.DepthClipEnable = false;
		rs.ScissorEnable = false;
		rs.MultisampleEnable = false;
		rs.AntialiasedLineEnable = false;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&rs, &g_pRasterizerState) )
			return false;

		g_pDevice->RSSetState(g_pRasterizerState);
	}

	// �p���v�x�}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
 	SAFE_RELEASE(g_pVertexLayout);
 	SAFE_RELEASE(g_pVertexBuffer);
 	SAFE_RELEASE(g_pSphereIndexBuffer);
 	SAFE_RELEASE(g_pConstantBuffer);
 	SAFE_RELEASE(g_pVertexShader);
 	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);
	SAFE_RELEASE(g_pTexture);

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
	UINT stride = sizeof(Vertex_VN);
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
	// �M�ζK��
	g_pDevice->PSSetShaderResources(0, 1, &g_pTexture);
	// �]�wvertex��Ʈ榡
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// �]�wvertex buffer
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// �]�windex buffer
	g_pDevice->IASetIndexBuffer(g_pSphereIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// �]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �p��x�}
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_proj_matrix = object_matrix * view_matrix * g_proj_matrix;
	// ��sshader�Ѽ�
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// �e�X��l
	//g_pDevice->Draw(4, 0);
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