#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutTexture_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;
static ID3D10InputLayout	*g_pQuadVertexLayout	= NULL;

static ID3D10Buffer			*g_pSunVertexBuffer		= NULL;
static ID3D10Buffer			*g_pEarthVertexBuffer	= NULL;
static ID3D10Buffer			*g_pMoonVertexBuffer	= NULL;
static ID3D10Buffer			*g_pSphereIndexBuffer	= NULL;
static ID3D10Buffer			*g_pQuadVertexBuffer = NULL;
static ID3D10Buffer			*g_pConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10VertexShader	*g_pTexVertexShader	= NULL;
static ID3D10PixelShader	*g_pTexPixelShader	= NULL;

static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10Texture2D *g_pTexture = NULL;
static ID3D10Texture2D *g_pDepthStencilTexture = NULL;
static ID3D10SamplerState	*g_pSamplerState = NULL;

static ID3D10ShaderResourceView *g_pTextureView = NULL;
static ID3D10RenderTargetView	*g_pRGBAView = NULL;
static ID3D10DepthStencilView	*g_pDepthStencilView = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	// ���J�e�P�y�Ϊ�Shader
	{
		ID3D10Blob *pVSCode = NULL;
		const char *shaer = "../../shaders/vertex_color_dx10.hlsl";
		// ���JVertex Shader
		g_pVertexShader = GutLoadVertexShaderDX10_HLSL(shaer, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader )
			return false;
		// ���JPixel Shader
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL(shaer, "PS", "ps_4_0");
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
	}

	// ���J�e�ʺA�K�ϥΪ�Shader
	{
		ID3D10Blob *pVSCode = NULL;
		const char *shader = "../../shaders/texture_dx10.hlsl";
		// ���JVertex Shader
		g_pTexVertexShader = GutLoadVertexShaderDX10_HLSL(shader, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pTexVertexShader )
			return false;
		// ���JPixel Shader
		g_pTexPixelShader = GutLoadPixelShaderDX10_HLSL(shader, "PS", "ps_4_0");
		if ( NULL==g_pTexPixelShader )
			return false;

		// �]�wVertex��Ʈ榡
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pQuadVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	D3D10_BUFFER_DESC cbDesc;
	D3D10_SUBRESOURCE_DATA sbDesc;

	{
		// sun vertex buffer
		cbDesc.ByteWidth = sizeof(Vertex_VC) * g_iNumSphereVertices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = 0;
		cbDesc.MiscFlags = 0;
		// �}��Vertex Buffer�ɦP�ɧ��ƫ����L�h
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
	}

	{
		// �]�w�@���Ψө�Index���O����.
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
	}

	{
		// vertex buffer
		cbDesc.ByteWidth = sizeof(g_Quad);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA subDesc;
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_Quad;
		// �t�m�@���i�H�s��Vertex���O����, �]�N�OVertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pQuadVertexBuffer ) )
			return false;
	}

	{
		// �t�mShader�`�ưO����
		cbDesc.ByteWidth = sizeof(Matrix4x4);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
			return false;
	}

	// �t�mRGBA�ʺA�K��
	{
		D3D10_TEXTURE2D_DESC dstex;
		dstex.Width = 512;
		dstex.Height = 512;
		dstex.MipLevels = 1;
		dstex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dstex.SampleDesc.Count = 1;
		dstex.SampleDesc.Quality = 0;
		dstex.Usage = D3D10_USAGE_DEFAULT;
		dstex.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		dstex.CPUAccessFlags = 0;
		dstex.MiscFlags = 0;
		dstex.ArraySize = 1;

		g_pDevice->CreateTexture2D( &dstex, NULL, &g_pTexture );

		D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
		SRVDesc.Format = dstex.Format;
		SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		// for shader
		g_pDevice->CreateShaderResourceView(g_pTexture, &SRVDesc, &g_pTextureView);

		D3D10_RENDER_TARGET_VIEW_DESC DescRT;
		DescRT.Format = dstex.Format;
		DescRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		DescRT.Texture2D.MipSlice = 0;
		// for rendertarget
		g_pDevice->CreateRenderTargetView(g_pTexture, &DescRT, &g_pRGBAView);

		D3D10_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));
		// �y�и�Ū�Ҧ���wrap
		sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
		// anisotropic filter
		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;

		g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);
	}

	// �t�mDepthStencil�ʺA�K��
	{
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );

		desc.Width =  512;
		desc.Height =  512;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;

		g_pDevice->CreateTexture2D( &desc, NULL, &g_pDepthStencilTexture );

		D3D10_DEPTH_STENCIL_VIEW_DESC target_desc;
		ZeroMemory( &target_desc, sizeof(target_desc) );

		target_desc.Format = desc.Format;
		target_desc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		target_desc.Texture2D.MipSlice = 0;

		g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture, &target_desc, &g_pDepthStencilView);
	}

	{
		// Rasterizer state����
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
	}

	// �p���ഫ�x�}
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

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
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);
}

static void RenderSolarSystem(void)
{
	UINT stride = sizeof(Vertex_VC);
	UINT offset = 0;

	// �]�wvertex shader
	g_pDevice->VSSetShader(g_pVertexShader);
	// �]�wpixel shader
	g_pDevice->PSSetShader(g_pPixelShader);
	// �]�wvertex��Ʈ榡
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// �]�windex buffer
	g_pDevice->IASetIndexBuffer(g_pSphereIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// �]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �p��X�@�ӥi�H�ഫ�����Y�y�Шt���x�}
	Vector4 eye(0.0f, 0.0f, 15.0f); 
	Vector4 lookat(0.0f, 0.0f, 0.0f); 
	Vector4 up(0.0f, 1.0f, 0.0f); 

	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 0.1f, 100.0f);

	Matrix4x4 view_matrix = GutMatrixLookAtRH(eye, lookat, up);
	Matrix4x4 view_proj_matrix = view_matrix * projection_matrix;
	Matrix4x4 world_view_proj_matrix;

	// sun
	world_view_proj_matrix = g_sun_matrix * view_proj_matrix;
	// �]�wshader�Ѽ�
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pSunVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// earth
	world_view_proj_matrix = g_earth_matrix * view_proj_matrix;
	// �]�wshader�Ѽ�
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pEarthVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);

	// moon
	world_view_proj_matrix = g_moon_matrix * view_proj_matrix;
	// �]�wshader�Ѽ�
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pMoonVertexBuffer, &stride, &offset);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);
}

void RenderFrameDX10(void)
{
	Vector4 vColorBlue(0.0f, 0.0f, 1.0f, 1.0f);
	Vector4 vColorGray(0.5f, 0.5f, 0.5f, 1.0f);

	// `���o�I�sGutCreateGraphicsDeviceDX10�ɩҲ��ͪ�D3D10����`
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	// front/back buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain();
	// `�]�wvertex shaderŪ���Ѽƪ��O������_`
    g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	ID3D10ShaderResourceView *null_views[4] = {NULL, NULL, NULL, NULL};

	// `��s�ʺA�K��`
	{
		g_pDevice->PSSetShaderResources(0, 4, null_views);
		g_pDevice->OMSetRenderTargets(1, &g_pRGBAView, g_pDepthStencilView);
		// `�M���C��`
		g_pDevice->ClearRenderTargetView(g_pRGBAView, (float *)&vColorGray);
		// `�M��`Depth/Stencil buffer
		g_pDevice->ClearDepthStencilView(g_pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

		RenderSolarSystem();

		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	}
	//  `�ϥΰʺA�K��`
	{
		UINT stride = sizeof(Vertex_VT);
		UINT offset = 0;
		// `�]�w`vertex shader
		g_pDevice->VSSetShader(g_pTexVertexShader);
		// `�]�w`pixel shader
		g_pDevice->PSSetShader(g_pTexPixelShader);
		// `�]�wvertex��Ʈ榡`
		g_pDevice->IASetInputLayout(g_pQuadVertexLayout);
		// `�M���C��`
		g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vColorBlue);
		// `�M��`Depth/Stencil buffer
		g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
		// `�]�w�T���γ��I���ޭȸ�ƱƦC�Otriangle strip`
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		// 
		g_pDevice->IASetVertexBuffers(0, 1, &g_pQuadVertexBuffer, &stride, &offset);
		// `�p��x�}`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
		Matrix4x4 world_view_proj_matrix = object_matrix * view_matrix * g_proj_matrix;
		// `��sshader�Ѽ�`
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = world_view_proj_matrix;
		g_pConstantBuffer->Unmap();
		// `�M�β�1�i�K��`
		g_pDevice->PSSetShaderResources(0, 1, &g_pTextureView);
		g_pDevice->PSSetSamplers(0, 1, &g_pSamplerState);
		// `�e�X�ݪO`
		g_pDevice->Draw(4, 0);
	}

	// ���ݵw�鱽����, �M��~��s�e��
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_