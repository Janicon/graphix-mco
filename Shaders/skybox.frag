#version 330 core

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform vec4 filterColor;
uniform int isFPP;

void main() {
	FragColor = texture(skybox, texCoords);
	
	if(isFPP == 1) {
		vec4 factor1 = vec4(1.0f) - FragColor;
		vec4 factor2 = vec4(1.0f) - FragColor.a;

		FragColor = (filterColor * factor1) - (FragColor * factor2);
		FragColor.a = 1.0f;
	}
	else {
		vec4 factor1 = filterColor;
		vec4 factor2 = FragColor;
	
		FragColor = (filterColor * factor1) + (FragColor * factor2);
	}
}