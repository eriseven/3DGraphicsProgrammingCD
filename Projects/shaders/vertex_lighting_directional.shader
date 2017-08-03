// `���I����Ʈ榡`
struct VS_INPUT
{
	float4 Position : POSITION;
	float3 Normal	: Normal;
};

// `�]�wVertex Shader��X����Ʈ榡`
struct VS_OUTPUT
{
	float4 Position : POSITION;
	float4 Color	: COLOR;
};

// `�ഫ�x�}`
uniform row_major float4x4 g_worldviewproj_matrix : register(c0);
uniform row_major float4x4 g_world_matrix : register(c4);
// `���Y��m`
uniform float4 g_camera_position  : register(c8);
// `���ҥ�`
uniform float4 g_Ambient : register(c12);
// `��V��`
uniform float3 g_Light_Direction : register(c13);
// `����M�����ۭ������G`
uniform float4 g_Light_Ambient : register(c14);
uniform float4 g_Light_Diffuse : register(c15);
uniform float4 g_Light_Specular : register(c16);
// `Specular���G�I�����{��`
uniform float  g_Light_Shininess : register(c17);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// `�ഫ��ù��y��`
	Out.Position = mul( In.Position, g_worldviewproj_matrix);
	
	// `�p��Normal, Position�b�@�ɮy�Шt�W����V���m`
	float3 vWorldNormal = mul( In.Normal, (float3x3) g_world_matrix);
	float3 vWorldPosition = mul( In.Position, g_world_matrix).xyz;
	// `���`
	float4 vAmbient = g_Ambient + g_Light_Ambient;
	// `��V����Diffuse����`
	float4 vDiffuse = g_Light_Diffuse * saturate(dot(g_Light_Direction, vWorldNormal));
	
	// `��V����Spcular����`
	float3 vCameraPosition = g_camera_position.xyz;
	float3 vCameraDir = normalize(vCameraPosition - vWorldPosition);
	float3 vHalfDir = normalize(g_Light_Direction + vCameraDir);
	float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
	float  fSpecular = pow(fSpecularCosine, g_Light_Shininess);
	float4 vSpecular = g_Light_Specular * fSpecular;
	
	// `�����[�_��`
	Out.Color = vAmbient + vDiffuse + vSpecular;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : COLOR
{
	// `�ϥγ��I�������X�Ӫ��C��`
	return In.Color;
}
