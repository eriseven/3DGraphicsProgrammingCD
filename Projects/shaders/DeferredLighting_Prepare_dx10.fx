/*
	Deferred Lighting ���ǳƤu�@, �⥴���һݭn����ƿ�X��ʺA�K�Ϥ�.
*/

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Texcoord : TEXCOORD0;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Texcoord : TEXCOORD0;
	float3 WorldPos : TEXCOORD1;
	float3 WorldNormal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 Binormal : TEXCOORD4;
};

SamplerState LinearSampler 
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4x4 wvp_matrix;
float4x4 world_matrix;

Texture2D diffuseTexture;
Texture2D normalmapTexture;

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position, 1.0f), wvp_matrix);
	Out.Texcoord = In.Texcoord;
	// `�@�ɮy�Шt�W�����I��m`
	Out.WorldPos = mul(float4(In.Position, 1.0f), world_matrix).xyz;
	// `�@�ɮy�Шt�W�� normal �� tangent space`
	Out.WorldNormal = mul(In.Normal, (float3x3) world_matrix);
	Out.Tangent = mul(In.Tangent, (float3x3) world_matrix);
	Out.Binormal = mul(In.Binormal, (float3x3) world_matrix);	
	
	return Out;
}

struct PS_OUTPUT
{
	// `Position ��b rendertarget0`
	float4 WorldPos : SV_Target; 
	
	// `Normal ��b rendertarget1`
	float4 WorldNormal : SV_Target1;
	
	// `�ϥ���O��b rendertarget2`
	float4 Diffuse : SV_Target2;
};

PS_OUTPUT PS(VS_OUTPUT In)
{
	PS_OUTPUT Out;

	// `���X normalmap`
	float3 normalmap = normalmapTexture.Sample(LinearSampler, In.Texcoord);
	normalmap = normalmap * 2.0f - 1.0f;
	
	float3x3 TangentSpace;
	TangentSpace[0] = normalize(In.Tangent);
	TangentSpace[1] = normalize(In.Binormal);
	TangentSpace[2] = normalize(In.WorldNormal);
	
	// '�� normalmap �V�q�ഫ��@�ɮy�Шt�W'
	float3 worldNormal = normalize(mul(normalmap, TangentSpace));
	
	// `���m, ���V, �ϥ��Ȥ��O��X�줣�P�� rendertarget �W.`
	Out.WorldPos = float4(In.WorldPos, 1.0f);
	Out.WorldNormal = float4(worldNormal, 1.0f);
	Out.Diffuse = diffuseTexture.Sample(LinearSampler, In.Texcoord);
		
	return Out;
}

BlendState MRT
{
	BlendEnable[0] = FALSE;
};

RasterizerState CullBack
{
	FrontCounterClockwise = TRUE;	
	CullMode = BACK;
};

DepthStencilState ZEnable
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

technique10 Prepare
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
	
        SetBlendState( MRT, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullBack);
        SetDepthStencilState(ZEnable, 0);
	}
}
