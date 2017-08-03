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

Texture2D FramebufferTexture;
SamplerState FramebufferSampler;

Texture2D BlurbufferTexture;
SamplerState BlurbufferSampler;

Texture2D ZBufferTexture;
SamplerState ZBufferSampler;

cbuffer VSConstants : register(b0)
{
	// �ഫ�x�}
	uniform row_major float4x4 wvp_matrix;
}

cbuffer PSConstants : register(b1)
{
	// Depth of Field Control
	uniform float4 vDepthOfField;
	// ��D�u�ʪ�Z���ഫ�^�����Y���u�ʶZ��
	uniform float4 z_inv;
}

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	// �y���ഫ
	Out.Position = mul( float4(In.Position, 1.0f), wvp_matrix);
	// Ū�����I���K��y��
	Out.Texcoord = In.Texcoord;
	//	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 framebuffer = FramebufferTexture.Sample(FramebufferSampler, In.Texcoord);
	float4 blurbuffer = BlurbufferTexture.Sample(BlurbufferSampler, In.Texcoord);
	float4 zbuffer = ZBufferTexture.Sample(ZBufferSampler, In.Texcoord);
	float4 z = float4(0.0f, 0.0f, zbuffer.r, 1.0f);
	float fLinearZ = abs(1.0f/dot(z, z_inv));

	float fNearBlur = saturate(vDepthOfField.x-fLinearZ)/vDepthOfField.y;
	float fFarBlur = saturate(fLinearZ - vDepthOfField.z)/vDepthOfField.w;
	//float fBlur = fFarBlur;
	float fBlur = max(fNearBlur, fFarBlur);
	
	float4 color = lerp(framebuffer, blurbuffer, fBlur);
		
	return color;
}
