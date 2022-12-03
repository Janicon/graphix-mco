// Draws light source model without lighting
#version 330 core //version

uniform vec3 color;

out vec4 FragColor;

void main() {
	FragColor = vec4(color, 1.f);
}