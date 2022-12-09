#version 330 core //version

struct DirectionLight {
	vec3 direction;
	vec3 color;
	float strength;
	float ambientStr;
	vec3 ambientColor;
	float specStr;
	float specPhong;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float linear;
	float quadratic;
	float ambientStr;
	vec3 ambientColor;
	float specStr;
	float specPhong;
};

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

uniform sampler2D tex0;
uniform vec3 cameraPos;

uniform DirectionLight directionLight;
uniform PointLight pointLight;

uniform int isFPP;
uniform vec4 filterColor;

out vec4 FragColor;

void main() {
	// Current pixel colors
	vec4 pixelColor = texture(tex0, texCoord);

	// Alpha cutoff
	if(pixelColor.a < 0.1)
		discard; // acts like return;

	// Manual implementation of glBlendFunc()
	if(isFPP == 1) {
		vec4 factor1 = vec4(1.0f) - pixelColor;
		vec4 factor2 = vec4(1.0f) - pixelColor.a;

		pixelColor = (filterColor * factor1) - (pixelColor * factor2);
		pixelColor.a = 1.0f;
	}
	
	// Lighting
	vec3 normal = normalize(normCoord);
	vec3 viewDir = normalize(cameraPos - fragPos);

	// Direction light calculations
	vec3 lightDir = normalize(-directionLight.direction);
	float diff = max(
		dot(normal, lightDir),
		0.0f
	);
	vec3 diffuse = diff * directionLight.color;
	vec3 ambientCol = directionLight.ambientStr * directionLight.ambientColor;
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(
		max(
			dot(reflectDir, viewDir), 0.1f
		),
		directionLight.specPhong
	);
	vec3 specCol = spec * directionLight.specStr * directionLight.color;
	// Save calculated direction light scaled by light strength
	vec3 result = (diffuse + ambientCol + specCol) * directionLight.strength;

	// Point light calculations
	lightDir = normalize(pointLight.position - fragPos);
	diff = max(
		dot(normal, lightDir),
		0.0f
	);

	diffuse = diff * pointLight.color;
	ambientCol = pointLight.ambientStr * pointLight.ambientColor;
	reflectDir = reflect(-lightDir, normal);
	spec = pow(
		max(
			dot(reflectDir, viewDir), 0.1f
		),
		pointLight.specPhong
	);
	specCol = spec * pointLight.specStr * pointLight.color;

	// Calculate distance of light to object to light
	float distance = length(pointLight.position - fragPos);
	// Calculate attenuation scaled by light strength
	float attenuation = 1.0f / (1.0f + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	// Scale lighting by attenuation value
	specCol *= attenuation;
	diffuse *= attenuation;
	ambientCol *= attenuation;


	// Add calculated point light to existing direction light colors
	result += (diffuse + ambientCol + specCol);

	FragColor = vec4(result, 1.0f) * pixelColor;
}