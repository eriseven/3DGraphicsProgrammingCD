// ���I����ƿ�J�榡
struct VS_INPUT
{
	float2 Texcoord : TEXCOORD0;
};

// Vertex Shader��X����Ʈ榡
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR;
	float2 Texcoord : TEXCOORD0;
	float PSize : PSIZE;
};

texture PositionTex;

sampler PositionSampler = sampler_state
{
	Texture = <PositionTex>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

texture VelocityTex;

sampler VelocitySampler = sampler_state
{
	Texture = <VelocityTex>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

float4x4 wvp_matrix;
float fTanW;
float2 ScreenSize;

//
// Vertex Shader
//
VS_OUTPUT VS_particle(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// `�q�K�Ϥ����X�ɤl��m��j�p`
	float4 position = tex2Dlod(PositionSampler, float4(In.Texcoord, 0, 0));
	// `�q�K�Ϥ����X�ɤl���ʳt�׸�ةR��`
	float4 velocity = tex2Dlod(VelocitySampler, float4(In.Texcoord, 0, 0));
	// `�g�Ѩ��X����m�Ӱ��y���ഫ`
	Out.Position = mul(float4(position.xyz, 1.0f), wvp_matrix);
	// �e�I�� PSize �i�H�M�w�@���I���j�p.
	Out.PSize = (ScreenSize.y * position.w) / (fTanW * abs(Out.Position.w));
	// `�ھڳѤU���ةR���u�ӨM�w�z����, ���֭n�����ͩR���ɤl�C�C����.`
	Out.Color = saturate(velocity.w * 5.0f);
	Out.Texcoord = 0;
	
	return Out;
}

texture DiffuseTex;

sampler DiffuseSampler = sampler_state
{
	Texture = <DiffuseTex>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;   
	AddressU  = Clamp;
	AddressV  = Clamp;
};

float4 PS_particle(VS_OUTPUT In) : COLOR
{
	return tex2D(DiffuseSampler, In.Texcoord) * In.Color;
}

technique RenderParticle
{
	pass p0 
	{
		VertexShader = compile vs_3_0 VS_particle();
		PixelShader = compile ps_3_0 PS_particle();

		AlphaTestEnable = TRUE;
		AlphaBlendEnable = TRUE;
		PointSpriteEnable = TRUE;
		SrcBlend = ONE;
		DestBlend = ONE;
		ZEnable = TRUE;
		ZWRITEENABLE = FALSE;
		CULLMODE = NONE;
	}
}
