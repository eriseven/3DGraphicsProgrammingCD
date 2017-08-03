#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "render_data.h"

#include "GutWin32.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;
static ID3D10Buffer			*g_pIndexBuffer		= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// ���JVertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/VertexCompression_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// ���JPixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/VertexCompression_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

    // �]�wVertex��Ʈ榡
    D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 4, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
    if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	// ���Ƽҫ����
	g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(CompressedVertex) * g_iNumSphereVertices, g_pCompressedVertices);
	g_pIndexBuffer = GutCreateIndexBuffer_DX10(sizeof(unsigned short) * g_iNumSphereIndices, g_pSphereIndices);
	g_pConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Matrix4x4)*2);
	
	// ��v�x�}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
 	SAFE_RELEASE(g_pVertexLayout);
 	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
 	SAFE_RELEASE(g_pVertexShader);
 	SAFE_RELEASE(g_pPixelShader);
 	SAFE_RELEASE(g_pConstantBuffer);

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
	Vector4 vClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	// ���o�I�sGutCreateGraphicsDeviceDX10�ɩҲ��ͪ�D3D10����
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	// �M���e��
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// �]�wshader
	g_pDevice->VSSetShader(g_pVertexShader);
	g_pDevice->PSSetShader(g_pPixelShader);
	// �]�wvertex shaderŪ���Ѽƪ��O������_
    g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// ��sshader�`�ưѼ�
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 *pShaderConstant;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pShaderConstant );
	pShaderConstant[0] = object_matrix * view_matrix * g_proj_matrix;
	pShaderConstant[1] = object_matrix;
	g_pConstantBuffer->Unmap();
	// �]�wvertex��Ʈ榡
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// �]�windex buffer & vertex buffer
	g_pDevice->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	UINT stride = sizeof(CompressedVertex);
	UINT offset = 0;
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// �]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// �e�X�y	
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);
	// ���ݵw�鱽����, �M��~��s�e��
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_