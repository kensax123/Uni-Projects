#version 330

uniform sampler2D texture;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void) {

	vec3 texColor = texture2D(texture, texCoord).rgb;
	
     // Output final gamma corrected colour to framebuffer
     // Note: It is assumed textures are loaded into the textured quad object in sRGB.  The 2.2 gamma correction is only an approximation of the correct sRGB curve, but it avoids a branch in treating different ranges of the input domain differently
    vec3 P = vec3(1.0 / 2.2);
    fragColour = vec4(pow(texColor, P), 1.0);
}
