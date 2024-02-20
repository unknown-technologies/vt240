#version 330

#define M_PI	3.14159265358979323846

const int width  = 800;
const int height = 240;

const float focus = 0.75;
const float glow_control = 0.9;
const float glow_intensity = 0.5;

uniform sampler2D vt240_screen;
uniform usampler2D vt240_mask;
uniform sampler2D blur_texture;

uniform bool enable_glow;

in  vec2 pos;
out vec4 color;

void main(void)
{
	// the VT220/VT240 has 240 lines
	float line = pos.y * float(height);
	float linebase = int(line);

	float lineoffset = line - linebase;
	float phi = 2.0 * M_PI * lineoffset;
	float intensity = pow((sin(phi) + 1.0) / 2.0, focus);

	// ... and 800 columns
	float column = pos.x * float(width);
	float colbase = int(column);

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
	uint mask0 = texelFetch(vt240_mask, texcoord0, 0).r;

	// second sample: current - 1
	ivec2 texcoord1 = texcoord0 - ivec2(1, 0);
	bool blank0 = texcoord1.x < 0;
	if(blank0) {
		texcoord1.x = 0;
	}

	vec4 fb1 = texelFetch(vt240_screen, texcoord1, 0);
	uint mask1 = texelFetch(vt240_mask, texcoord1, 0).r;
	if(blank0) {
		// blank it if we are outside of the screen
		fb1 = vec4(0.0);
		mask1 = 0u;
	}

	// third sample: current - 2
	ivec2 texcoord2 = texcoord1 - ivec2(1, 0);
	if(texcoord2.x < 0) {
		texcoord2.x = 0;
	}

	vec4 fb2 = texelFetch(vt240_screen, texcoord2, 0);
	uint mask2 = texelFetch(vt240_mask, texcoord2, 0).r;

	// combine the samples with phosphor simulation
	vec4 result;
	if(mask1 > mask0) {
		// sequence 'X10' => '1'
		result = fb1;
	} else if(mask2 > mask0) {
		// sequence '100' => '0'
		result = fb0 * vec4(xblend) + fb2 * vec4(1.0 - xblend);
	} else if(mask2 > mask1) {
		// sequence '101' => '1'
		result = fb0 * vec4(xblend) + fb2 * vec4(1.0 - xblend);
	} else {
		// sequence 'X01' => '1'
		result = fb0 * vec4(xblend) + fb1 * vec4(1.0 - xblend);
	}

	// get glow
	vec4 glow = pow(texture(blur_texture, pos), vec4(glow_control));

	if(!enable_glow) {
		glow = vec4(0.0);
	}

	// combine VT240 screen with scanlines and a bit of glow
	vec4 vt240 = (result + pow(glow, vec4(2.0)) * 0.2) * vec4(intensity);

	// combine with glow for the final result
	color = vec4((vt240 + glow * glow_intensity).rgb, 1.0);
}
