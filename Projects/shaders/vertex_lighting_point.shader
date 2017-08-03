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
// `�I����`
uniform float3 g_Light_Position : register(c13);
uniform float3 g_Light_Attenuation : register(c14);
// `����M�����ۭ������G`
uniform float4 g_Light_Ambient : register(c15);
uniform float4 g_Light_Diffuse : register(c16);
uniform float4 g_Light_Specular : register(c17);
// `Specular���G�I�����{��`
uniform float  g_Light_Shininess : register(c18);

// Vertex Shader
VS_OUTPUT VS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	// `�ഫ��ù��y��`
	Out.Position = mul( In.Position, g_worldviewproj_matrix);
	
	// `�p��Normal, Position�b�@�ɮy�Шt�W����V���m.`
	float3 vWorldNormal = mul( In.Normal, (float3x3) g_world_matrix);
	float3 vWorldPosition = mul( In.Position, g_world_matrix).xyz;
	
	// `���`
	float4 vAmbient = g_Ambient + g_Light_Ambient;
	
	// `�p�������V��Z��`
	float3 vDiff = g_Light_Position - vWorldPosition;
	float  fDistance = length(vDiff);
	float3 vDir = vDiff / fDistance;
	
	// `������Diffuse����`
	float4 vDiffuse = g_Light_Diffuse * saturate(dot(vDir, vWorldNormal));
	
	// `�跽��Spcular����`
	float3 vCameraPosition = g_camera_position.xyz;
	float3 vCameraDir = normalize(vCameraPosition - vWorldPosition);
	float3 vHalfDir = normalize(vDir + vCameraDir);
	float  fSpecularCosine = saturate(dot(vHalfDir, vWorldNormal));
	float  fSpecular = pow(fSpecularCosine, g_Light_Shininess);
	float4 vSpecular = g_Light_Specular * fSpecular;
	
	// `�p������I��`
	float fLightAttenuation = dot(float3(1.0f, fDistance, fDistance * fDistance), g_Light_Attenuation);
	
	// `�����[�_��`
	Out.Color = vAmbient + (vDiffuse + vSpecular) / fLightAttenuation;
	
	return Out;
}

// Pixel Shader
float4 PS(VS_OUTPUT In) : COLOR
{
	// `�ϥγ��I�������X�Ӫ��C��`
	return In.Color;
}
