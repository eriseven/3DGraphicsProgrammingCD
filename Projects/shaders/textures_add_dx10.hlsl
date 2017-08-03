// �]�w���I����Ʈ榡
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Texcoord	: TEXCOORD;
};

// �]�wVertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 Texcoord	: TEXCOORD0;
};

Texture2D InputTextureA;
SamplerState InputTextureASampler;

Texture2D InputTextureB;
SamplerState InputTextureBSampler;

// �ഫ�x�}
uniform row_major float4x4 viewproj_matrix;

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// �y���ഫ
	Out.Position = mul( float4(In.Position, 1.0f), viewproj_matrix);
	// Ū�����I���K��y��
	Out.Texcoord = In.Texcoord;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 p0 = InputTextureA.Sample(InputTextureASampler, In.Texcoord);
	float4 p1 = InputTextureB.Sample(InputTextureBSampler, In.Texcoord);
	float4 color = p0 + p1;
		
	return color;
}
