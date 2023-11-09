uniform vec3 u_camera_pos;
uniform sampler3D u_texture;
uniform mat4 u_model;
uniform mat4 u_invmodel;
uniform float u_step_length;


varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;


void main() {

    float d;
	vec3 dir = normalize(v_position - (u_invmodel*vec4(u_camera_pos, 1.0)).xyz);
    vec3 sample_pos = v_position;
    vec3 step_vec = dir*u_step_length;


	vec4 final_color = vec4(0.0);
    vec4 sample_color;


	for (int i = 0; i < 10000; i++) {

		d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;	// coords between 0 and 1 ok
		
		sample_color = vec4(d,d,d,d);
        sample_color.rgb *= sample_color.a;

		final_color += u_step_length*(1.0-final_color.a)*sample_color;	

	    sample_pos += step_vec;
		if (final_color.a >= 1.0) {
				final_color = vec4(1.0,0.0,0.0,0.0);
		}

		if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
			break;
		}
	}

	gl_FragColor = final_color;		// multiply by brightness

}





// añadir step_length cuando se vea algo :(
