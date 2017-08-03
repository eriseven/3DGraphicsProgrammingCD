/*
	`Deferred Lighting �����p��`
*/

struct VS_INPUT
{
	float3 Position : POSITION;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float4 Texcoord : TEXCOORD;
};

float4x4 wvp_matrix;

Texture2D worldposTexture;
Texture2D worldnormalTexture;
Texture2D diffuseTexture;

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul(float4(In.Position.xyz, 1.0f), wvp_matrix);
	Out.Texcoord = Out.Position;
	
	return Out;
}

float4 vCameraPos;
float4 vLightPos;
float4 vLightColor;
float4 vLightAmbient;

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// `�p���I������ Pixel Shader`
float4 PS_PointLight(VS_OUTPUT In) : SV_Target
{
	// `��ù��y�Шt�������K�Ϯy��, ���u�O²�檺�����[�W�Y��.`
	float2 vTexcoord = (In.Texcoord.xy / In.Texcoord.w) * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	
	// `�q�K�Ϥ����X�o�ӹ�������m, ���V, ��ϥ���O.`
	float3 vWorldPos = worldposTexture.Sample(PointSampler, vTexcoord).xyz;
	float3 vWorldNormal = worldnormalTexture.Sample(PointSampler, vTexcoord).xyz;
	float4 vAlbedo = diffuseTexture.Sample(PointSampler, vTexcoord);
	
	// `�p�⥴��`
	float3 vLightDir = normalize(vLightPos - vWorldPos);
	float3 vCameraDir = normalize(vCameraPos - vWorldPos);
	float3 vHalfVector = normalize(vCameraDir + vLightDir);
	
	float  fDistance = distance(vLightPos, vWorldPos);
	float  fAttenuation = 1.0f-fDistance/vLightPos.w;
	
	float4 vDiffuse = vLightColor * saturate(dot(vLightDir, vWorldNormal));
	float4 vSpecular = vLightColor * pow(saturate(dot(vHalfVector, vWorldNormal)), 30.0f) * vAlbedo.a;
	float4 vLighting = (vDiffuse + vSpecular ) * vAlbedo * fAttenuation;
	
	float4 nml = (vWorldNormal.xyzz + 1.0f) * 0.5f;
	
	return vLighting;
}

// `�p�� Ambient Light �� Pixel Shader`
float4 PS_AmbientLight(VS_OUTPUT In) : SV_Target
{
	// `��ù��y�Шt�������K�Ϯy��, ���u�O²�檺�����[�W�Y��.`
	float2 vTexcoord = (In.Texcoord.xy / In.Texcoord.w) * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	
	float4 vAlbedo = diffuseTexture.Sample(PointSampler, vTexcoord);
	float4 vAmbient = vLightColor * vAlbedo;
			
	return vAmbient;
}

RasterizerState CullBack
{
	FrontCounterClockwise = TRUE;	
	CullMode = BACK;
};

BlendState AddBlend
{
	BlendEnable[0] = TRUE;
	SrcBlend = ONE;
	DestBlend = ONE;
};

DepthStencilState ZWriteDisable
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    DepthFunc = Less_Equal;
};

technique10 PointLight
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_PointLight() ) );
        
        SetRasterizerState(CullBack);
        SetBlendState(AddBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState(ZWriteDisable, 0);
	}
}

technique10 AmbientLight
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_AmbientLight() ) );

        SetBlendState(AddBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState(ZWriteDisable, 0);
	}
}

struct VS_INPUT_VC
{
	float4 Position : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT_VC
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT_VC VS_VC(VS_INPUT_VC In)
{
	VS_OUTPUT_VC Out;
	
	Out.Position = mul(float4(In.Position.xyz, 1.0f), wvp_matrix);
	Out.Color = In.Color;
	
	return Out;
}

float4 PS_COLOR(VS_OUTPUT_VC In) : SV_Target
{
	return In.Color;
}

technique10 DrawLight
{
	pass p0 
	{
        SetVertexShader( CompileShader( vs_4_0, VS_VC() ) );
        SetPixelShader( CompileShader( ps_4_0, PS_COLOR() ) );
	}
}
