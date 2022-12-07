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

uniform vec3 pointLight_position;
uniform vec3 pointLight_color;
uniform float pointLight_strength;
uniform float pointLight_ambientStr;
uniform vec3 pointLight_ambientColor;
uniform float pointLight_specStr;
uniform float pointLight_specPhong;

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

	// Point light calculations
	lightDir = normalize(pointLight_position - fragPos);
	diff = max(
		dot(normal, lightDir),
		0.0f
	);
	diffuse = diff * pointLight_color;
	ambientCol = pointLight_ambientStr * pointLight_ambientColor;
	reflectDir = reflect(-lightDir, normal);
	spec = pow(
		max(
			dot(reflectDir, viewDir), 0.1f
		),
		pointLight_specPhong
	);
	specCol = spec * pointLight_specStr * pointLight_color;

	// Calculate distance of light to object to light
	float distance = length(pointLight_position - fragPos);
	// Calculate attenuation scaled by light strength
	float attenuation = 1.0 / (distance * distance) * pointLight_strength;

	// Scale lighting by attenuation value
	specCol *= attenuation;
	diffuse *= attenuation;
	ambientCol *= attenuation;

	// Add calculated point light to existing direction light colors
	result += specCol + diffuse + ambientCol;

	FragColor = vec4(result, 1.0f) * pixelColor;
}