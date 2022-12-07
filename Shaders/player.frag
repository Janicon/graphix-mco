#version 330 core //version

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec3 cameraPos;

uniform vec3 dirLight_direction;
uniform vec3 dirLight_color;
uniform float dirLight_strength;
uniform float dirLight_ambientStr;
uniform vec3 dirLight_ambientColor;
uniform float dirLight_specStr;
uniform float dirLight_specPhong;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;
in mat3 TBN;

out vec4 FragColor;

void main() {
	// Current pixel colors
	vec4 pixelColor = texture(tex0, texCoord);
	
	// Alpha cutoff
	if(pixelColor.a < 0.1)
		discard; // acts like return;
	
	// Lighting
	//vec3 normal = normalize(normCoord);
	vec3 normal = texture(tex1, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal);

	vec3 viewDir = normalize(cameraPos - fragPos);

	// Direction light calculations
	vec3 lightDir = normalize(-dirLight_direction);
	float diff = max(
		dot(normal, lightDir),
		0.0f
	);
	vec3 diffuse = diff * dirLight_color;
	vec3 ambientCol = dirLight_ambientStr * dirLight_ambientColor;
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(
		max(
			dot(reflectDir, viewDir), 0.1f
		),
		dirLight_specPhong
	);
	vec3 specCol = spec * dirLight_specStr * dirLight_color;
	// Save calculated direction light scaled by light strength
	vec3 result = (diffuse + ambientCol + specCol) * dirLight_strength;

	FragColor = vec4(result, 1.0f) * pixelColor;
}