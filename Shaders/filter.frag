#version 330 core //version

uniform sampler2D tex0;

in vec2 texCoord;

out vec4 FragColor;

void main() {
	// Current pixel colors
	vec4 pixelColor = texture(tex0, texCoord);
	
	// Alpha cutoff
	if(pixelColor.a < 0.1)
		discard; // acts like return;

	FragColor = vec4(1.0f) * pixelColor;
}