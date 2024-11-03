#version 330

#define M_PI	3.14159265358979323846

const int width  = 800;
const int height = 240;

uniform float focus = 0.75;
uniform float glow_control = 0.9;
uniform float glow_intensity = 0.5;

uniform sampler2D vt240_screen;
uniform usampler2D vt240_mask;
uniform sampler2D blur_texture;

uniform bool enable_glow;
uniform bool raw_mode;

in  vec2 pos;
out vec4 color;

void main(void)
{
	// the VT220/VT240 has 240 lines
	float line = pos.y * float(height);
	float linebase = float(int(line));

	float lineoffset = line - linebase;
	float phi = 2.0 * M_PI * lineoffset;
	float intensity = pow((sin(phi) + 1.0) / 2.0, focus);

	// ... and 800 columns
	float column = pos.x * float(width);
	float colbase = float(int(column));

	float xblend = column - colbase;

	// jump to next line if we are over 75% of the line
	int texline = int(linebase);
	if(lineoffset > 0.75 && (texline + 1) < height) {
		texline++;
	}

	// blank last line / avoid 25% of incorrect color
	if(lineoffset > 0.75 && (texline + 1) == height) {
		intensity = 0.0;
	}

	// retrieve 3 samples for phosphor simulation with bit extension
	// first sample: current pixel
	ivec2 texcoord0 = ivec2(int(colbase), texline);
	vec4 fb0 = texelFetch(vt240_screen, texcoord0, 0);

	// second sample: current - 1
	ivec2 texcoord1 = texcoord0 - ivec2(1, 0);
	bool blank0 = texcoord1.x < 0;
	if(blank0) {
		texcoord1.x = 0;
	}

	vec4 fb1 = texelFetch(vt240_screen, texcoord1, 0);
	if(blank0) {
		// blank it if we are outside of the screen
		fb1 = vec4(0.0);
	}

	vec4 result = mix(fb1, fb0, xblend);

	// get glow
	vec4 glow = pow(texture(blur_texture, pos), vec4(glow_control));

	if(!enable_glow) {
		glow = vec4(0.0);
	}

	// combine VT240 screen with scanlines and a bit of glow
	vec4 vt240 = (result + pow(glow, vec4(2.0)) * 0.2) * vec4(intensity);

	if(raw_mode) {
		color = vec4(fb0.rgb, 1.0);
	} else {
		// combine with glow for the final result
		color = vec4((vt240 + glow * glow_intensity).rgb, 1.0);
	}
}
