#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutModel_DX10.h"
#include "render_data.h"

#include "GutWin32.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pBorderVertexBuffer = NULL;

static ID3D10Buffer			*g_pSphereIndexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static Matrix4x4 g_proj_matrix;
static CGutModel_DX10 g_Model_DX10[3];

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

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");
	for ( int i=0; i<3; i++ )
	{
		g_Model_DX10[i].ConvertToDX10Model(&g_Models[i]);
	}

    // �]�wVertex��Ʈ榡
    D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

    if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	//
	g_pBorderVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(g_Border), g_Border);
	// �t�mShaderŪ���Ѽƪ��O����Ŷ�
	g_pConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Matrix4x4));

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(70.0f, 1.0f, 0.1f, 100.0f);

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
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(70.0f, aspect, 0.1f, 100.0f);
}

void RenderSolarSystemDX10(Matrix4x4 &view_matrix, Matrix4x4 &proj_matrix)
{
	UINT stride = sizeof(Vertex_VC);
	UINT offset = 0;

	// �]�wvertex��Ʈ榡
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// �]�windex buffer
	g_pDevice->IASetIndexBuffer(g_pSphereIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// �]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CGutModel_DX10::SetProjectionMatrix(proj_matrix);
	CGutModel_DX10::SetViewMatrix(view_matrix);

	// sun
	{
		g_Model_DX10[0].SetWorldMatrix(g_sun_matrix);
		g_Model_DX10[0].UpdateMatrix();
		g_Model_DX10[0].Render();
	}
	// earth
	{
		g_Model_DX10[1].SetWorldMatrix(g_earth_matrix);
		g_Model_DX10[1].UpdateMatrix();
		g_Model_DX10[1].Render();
	}
	// moon
	{
		g_Model_DX10[2].SetWorldMatrix(g_moon_matrix);
		g_Model_DX10[2].UpdateMatrix();
		g_Model_DX10[2].Render();
	}
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	// frame buffer
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	// depth/stencil buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	// front/back buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 

	// `�M���C��`
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// `�M��`Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	int w, h;
	GutGetWindowSize(w, h);

	Matrix4x4 view_matrix;
	Matrix4x4 ortho_proj = GutMatrixOrthoRH_DirectX(20.0f, 20.0f, 0.1f, 100.0f);

	// `�e����`
	{
		D3D10_VIEWPORT vp = {0, 0, w/2, h/2, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &vp);
		// view matrix
		Vector4 camera_pos(0.0f, -20.0f, 0.0f);
		Vector4 camera_up(0.0f, 0.0f, 1.0f);
		view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		// render objects
		RenderSolarSystemDX10(view_matrix, ortho_proj);
	}
	// `�W����`
	{
		D3D10_VIEWPORT vp = {w/2, 0, w/2, h/2, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &vp);
		// view matrix
		Vector4 camera_pos(0.0f, 0.0f, 20.0f);
		Vector4 camera_up(0.0f, 1.0f, 0.0f);
		view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		// render objects
		RenderSolarSystemDX10(view_matrix, ortho_proj);
	}
	// `�k����`
	{
		D3D10_VIEWPORT vp = {0, h/2, w/2, h/2, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &vp);
		// view matrix
		Vector4 camera_pos(20.0f, 0.0f, 0.0f);
		Vector4 camera_up(0.0f, 0.0f, 1.0f);
		view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		// render objects
		RenderSolarSystemDX10(view_matrix, ortho_proj);
	}
	// `�ϥΪ̵���`
	{
		D3D10_VIEWPORT vp = {w/2, h/2, w/2, h/2, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &vp);
		// view matrix
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
		view_matrix = object_matrix * view_matrix;
		// render objects
		RenderSolarSystemDX10(view_matrix, g_proj_matrix);
	}
	// `�e��u`
	{
		UINT stride = sizeof(Vertex_VC);
		UINT offset = 0;

		D3D10_VIEWPORT vp = {0, 0, w, h, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &vp);

		// `�]�w`vertex shader
		g_pDevice->VSSetShader(g_pVertexShader);
		// `�]�w`pixel shader
		g_pDevice->PSSetShader(g_pPixelShader);
		// `�]�wvertex shaderŪ���Ѽƪ��O�����m`
		g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->Identity();
		g_pConstantBuffer->Unmap();

		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pBorderVertexBuffer, &stride, &offset);
		g_pDevice->Draw(4, 0);
	}
	// `���ݵw�鱽����, �M��~��s�e��.`
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_