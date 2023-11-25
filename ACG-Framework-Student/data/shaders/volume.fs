uniform vec3 u_camera_position;
uniform sampler3D u_texture;
uniform mat4 u_model;
uniform float u_step_length;
uniform float u_brightness;
uniform vec4 u_color;
uniform sampler2D u_noise_txt;
uniform sampler2D u_transfer_function;
uniform bool u_jittering_text;
uniform bool u_jittering_rand;
uniform vec4 u_clipping_plane;
uniform bool u_clipping1;
uniform bool u_clipping2;
uniform bool u_transfer_bool;
uniform float u_threshold;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

float rand(vec2 co) {
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {

    float d, offset;
	vec2 noise_coords = gl_FragCoord.xy/128.0;

	if (u_jittering_text) {
		offset = texture2D(u_noise_txt, noise_coords).z;	
	} else if (u_jittering_rand) {
		offset = rand(gl_FragCoord.xy);
	} else {
		offset = 0;
	}

	vec3 dir = normalize(v_position.xyz - u_camera_position.xyz);
	vec3 sample_pos = v_position.xyz + offset*dir;
	vec3 step_vec = dir*u_step_length;

	vec4 final_color = vec4(0.0), sample_color;
	float value;
	for (int i = 0; i < 1000000; i++) {

		if (u_clipping1 || u_clipping2) {
			value = dot(u_clipping_plane.xyz, sample_pos.xyz) + u_clipping_plane.a;
			if ((u_clipping1) && (value < 0 )) {
				sample_pos += step_vec;
				if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
					break;
				}
				continue;
			}
			else if ((u_clipping2) && (value > 0 )) {
				sample_pos += step_vec;
				if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
					break;
				}
				continue;
			}
		}

		d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
		if (d < u_threshold) {
			if (u_transfer_bool) {
				sample_color = texture2D(u_transfer_function, vec2(d,1));
			} else {
				sample_color = vec4(d,d,d,d);
			}

			sample_color.rgb *= sample_color.a;

			final_color += u_step_length*(1.0-final_color.a)*sample_color;

			if (final_color.a >= 0.9) {
				break;
			}
		}
		sample_pos += step_vec;
		if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
			break;
		}
	}

	if (final_color.a < 0.03) {
		discard;
	}

	gl_FragColor = u_brightness*final_color*u_color;
}
