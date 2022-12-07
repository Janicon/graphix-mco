#version 330 core //version

uniform sampler2D tex0;
uniform vec3 cameraPos;

uniform vec3 dirLight_direction;
uniform vec3 dirLight_color;
uniform float dirLight_strength;
uniform float dirLight_ambientStr;
uniform vec3 dirLight_ambientColor;
uniform float dirLight_specStr;
uniform float dirLight_specPhong;

uniform vec3 spotLight_position;
uniform vec3 spotLight_color;
uniform float spotLight_linear;
uniform float spotLight_quadratic;
uniform float spotLight_ambientStr;
uniform vec3 spotLight_ambientColor;
uniform float spotLight_specStr;
uniform float spotLight_specPhong;
uniform float cutoff;
uniform float outercutoff;
in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;

void main() {
	// Current pixel colors
	vec4 pixelColor = texture(tex0, texCoord);
	
	// Alpha cutoff
	if(pixelColor.a < 0.1)
		discard; // acts like return;
	
	// Lighting
	vec3 normal = normalize(normCoord);
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

	// Spot light calculations
	float theta = dot(viewDir, normalize(-spotLight_position));
	float epsilon = cutoff - outercutoff;
	float intensity = clamp((theta - outercutoff) / epsilon, 0.0, 1.0); 

	// spot light calculations
	lightDir = normalize(spotLight_position - fragPos);
	diff = max(
		dot(normal, lightDir),
		0.0f
	);
	diffuse = diff * spotLight_color;
	ambientCol = spotLight_ambientStr * spotLight_ambientColor;
	reflectDir = reflect(-lightDir, normal);
	spec = pow(
		max(
			dot(reflectDir, viewDir), 0.1f
		),
		spotLight_specPhong
	);
	specCol = spec * spotLight_specStr * spotLight_color;

	specCol *= intensity;
	diffuse *= intensity;

	// Calculate distance of light to object to light
	float distance = length(spotLight_position - fragPos);
	// Calculate attenuation scaled by light strength
	float attenuation = 1.0f / 1.0f + spotLight_linear * (distance * distance) + spotLight_quadratic;

	// Scale lighting by attenuation value
	specCol *= attenuation;
	diffuse *= attenuation;
	ambientCol *= attenuation;


	// Add calculated spot light to existing direction light colors
	result += (diffuse + ambientCol + specCol);

	FragColor = vec4(result, 1.0f) * pixelColor;
}