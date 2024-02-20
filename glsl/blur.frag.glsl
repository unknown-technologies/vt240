#version 330

uniform sampler2D image;
uniform vec2 direction;

in  vec2 pos;
out vec4 color;

vec4 blur13(vec2 uv) {
	vec2 resolution = vec2(textureSize(image, 0));

	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.411764705882353)  * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294)  * direction;

	color += texture(image, uv) * 0.1964825501511404;
	color += texture(image, uv + (off1 / resolution)) * 0.2969069646728344;
	color += texture(image, uv - (off1 / resolution)) * 0.2969069646728344;
	color += texture(image, uv + (off2 / resolution)) * 0.09447039785044732;
	color += texture(image, uv - (off2 / resolution)) * 0.09447039785044732;
	color += texture(image, uv + (off3 / resolution)) * 0.010381362401148057;
	color += texture(image, uv - (off3 / resolution)) * 0.010381362401148057;

	return color;
}

void main(void)
{
	color = blur13(pos);
}
