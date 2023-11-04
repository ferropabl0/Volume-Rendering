uniform vec3 u_camera_pos;
uniform sampler3D u_texture;

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

void main()
{
	// ray
	vec3 dir = normalize(v_position - u_camera_pos);
	
	float step_length = 0.3;
	vec3 sample_pos = v_position;
	vec4 final_color = vec4(0.0);
	vec3 step_vec = dir*step_length;

	float d;
	vec4 sample_color;
	
	for (int i = 0; i < 10000; i++) {
		d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
	    	sample_color = vec4(d,1.0-d,0,d*d);6
		sample_color.xyz *= sample_color.a;
		
	    	final_color += (1.0-final_color.a)*sample_color;
	    	if (final_color.a >= 0.98) {
			break;
	    	}
	    	sample_pos += step_vec;
		/*if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {
			break;
		}*/
	}

	final_color *= step_length;

	gl_FragColor = final_color;		// multiply by brightness
}
