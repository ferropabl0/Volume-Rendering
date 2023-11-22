uniform vec3 u_camera_position;
uniform sampler3D u_texture;
uniform mat4 u_model;
uniform float u_step_length;
uniform float u_brightness;
uniform vec4 u_color;
uniform sampler2D u_noise_txt;
uniform sampler2D u_transfer_function;
uniform int u_jittering_mode;

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

float rand(vec2 co) {
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {

    float d, offset;
	vec2 noise_coords = gl_FragCoord.xy/128.0;

	/*if (u_jittering_mode == 1) {
		offset = texture2D(u_noise_txt, noise_coords).z;	
	} else if (u_jittering_mode == 2) {
		offset = rand(gl_FragCoord.xy);
	} else if (u_jittering_mode == 0) {
		offset = 0;
	}*/
	offset = 0;
	vec3 dir = normalize(v_position.xyz - u_camera_position.xyz);
	vec3 sample_pos = v_position.xyz + offset*dir;
	vec3 step_vec = dir*u_step_length;

	vec4 final_color = vec4(0.0), sample_color;
	vec4 aux_color;
	vec4 plane = vec4(3,1,1,1);
	float value;
	for (int i = 0; i < 1000000; i++) {

		d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;

		value = dot(plane.xyz, sample_pos.xyz) + plane.a;
		/* if (value < 0 ) {
			sample_pos += step_vec;
			if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
				break;
			}
			continue;
		} */
		aux_color = texture2D(u_transfer_function, vec2(d,1));
		sample_color = vec4(aux_color.x, aux_color.y, aux_color.z, d);

		sample_color.rgb *= sample_color.a;

	 	final_color += u_step_length*(1.0-final_color.a)*sample_color;

	 	if (final_color.a >= 0.9) {
	 	 	break;
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
