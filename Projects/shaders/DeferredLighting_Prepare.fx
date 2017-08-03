/*
	Deferred Lighting ���ǳƤu�@, �⥴���һݭn����ƿ�X��ʺA�K�Ϥ�.
*/

struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Texcoord : TEXCOORD0;
	float3 Tangent : TEXCOORD1;
	float3 Binormal : TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
	float3 WorldPos : TEXCOORD1;
	float3 WorldNormal : TEXCOORD2;
	float3 Tangent : TEXCOORD3;
	float3 Binormal : TEXCOORD4;
};

float4x4 wvp_matrix;
float4x4 world_matrix;

texture diffuseTexture;
sampler2D diffuseSampler : register(s0) = sampler_state
{
	texture = <diffuseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Wrap;
	AddressV  = Wrap;
};

texture normalmapTexture;
sampler2D normalmapSampler : register(s1) = sampler_state
{
	texture = <normalmapTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Wrap;
	AddressV  = Wrap;
};

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
	float4 WorldPos : COLOR0; 
	// `Normal ��b rendertarget1`
	float4 WorldNormal : COLOR1;
	// `�ϥ���O��b rendertarget2`
	float4 Diffuse : COLOR2;
};

PS_OUTPUT PS(VS_OUTPUT In)
{
	PS_OUTPUT Out;

	float3x3 TangentSpace;
	// `���X normalmap`
	float3 normalmap = tex2D(normalmapSampler, In.Texcoord);
	normalmap = normalmap * 2.0f - 1.0f;
	
	TangentSpace[0] = normalize(In.Tangent);
	TangentSpace[1] = normalize(In.Binormal);
	TangentSpace[2] = normalize(In.WorldNormal);
	// '�� normalmap �V�q�ഫ��@�ɮy�Шt�W'
	float3 worldNormal = mul(normalmap, TangentSpace);
	
	// `���m, ���V, �ϥ��Ȥ��O��X�줣�P�� rendertarget �W.`
	Out.WorldPos = float4(In.WorldPos, 1.0f);
	Out.WorldNormal = float4(worldNormal, 1.0f);
	Out.Diffuse = tex2D(diffuseSampler, In.Texcoord);
		
	return Out;
}

technique Prepare
{
	pass p0 
	{
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
		
		ALPHABLENDENABLE = FALSE;
		ALPHATESTENABLE = FALSE;
		ZENABLE = TRUE;
		ZWRITEENABLE = TRUE;
	}
}
