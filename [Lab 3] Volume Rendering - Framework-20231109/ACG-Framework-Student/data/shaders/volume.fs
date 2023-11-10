uniform vec3 u_camera_pos;
uniform sampler3D u_texture;
uniform mat4 u_model;
uniform mat4 u_invmodel;
uniform float u_step_length;
uniform float u_brightness;


varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;


void main() {

    float d;
	vec3 dir = normalize(v_position - (u_invmodel*vec4(u_camera_pos, 1.0)).xyz), sample_pos = v_position, step_vec = dir*u_step_length;


	vec4 final_color = vec4(0.0), sample_color;

	for (int i = 0; i < 10000; i++) {

		d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
		
		sample_color = vec4(d,d,d,d);
		sample_color.rgb *= sample_color.a;

	 	final_color += u_step_length*(1.0-final_color.a)*sample_color;

	 	if (final_color.a >= 0.99) {
	 		// final_color = vec4(0.0,1.0,0.0,0.0);
	 	 	break;
	 	}
 
		sample_pos += step_vec;
		if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
	 	  	// final_color = vec4(1.0,0.0,0.0,0.0);
	 	  	break;
		}
	}

	gl_FragColor = u_brightness*final_color;
}





// añadir step_length cuando se vea algo :(
