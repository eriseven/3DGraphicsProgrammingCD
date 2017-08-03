// ���ܫG��

// ���I����ƿ�J�榡
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord : TEXCOORD;
};

// Vertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float2 Texcoord : TEXCOORD0;
};

sampler2D RGBASampler : register(s0);

// VS �ഫ�x�}
cbuffer Matrix : register(b0)
{
	uniform row_major float4x4 wvp_matrix;
}

// PS �վ�G�װѼ�
cbuffer Brightness : register(b1)
{
	uniform float4 IntensityOffset;
	uniform float4 IntensityScale;
}

//
// Vertex Shader
//
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	Out.Texcoord = In.Texcoord;

	return Out;
}

//
// Pixel Shader
//
float4 PS(VS_OUTPUT In) : COLOR
{
	float4 rgba = tex2D(RGBASampler, In.Texcoord);
	float3 table = float3(0.3f, 0.59f, 0.11f);
	// ���ন�Ƕ��ӹ�ݥ����G��
	float4 old_intensity = dot(rgba.rgb, table);
	// ��G�װ��ӧ���
	float4 new_intensity = (old_intensity + IntensityOffset) * IntensityScale;
	// ���l���C�⭼�W�s���G��
	float4 color = rgba * new_intensity;
	
	return color;
}
