uniform vec3 u_camera_position;
uniform sampler3D u_texture;
uniform mat4 u_model;
uniform float u_step_length;
uniform float u_brightness;
uniform vec4 u_color;
uniform float u_threshold;
uniform float u_h;
uniform vec3 u_light_pos;


varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;


void main() {

    float d;

	vec3 dir = normalize(v_position.xyz - u_camera_position.xyz);
	vec3 sample_pos = v_position.xyz;
	vec3 step_vec = dir*u_step_length;

	vec3 gradient, x_pos1, y_pos1, z_pos1, x_pos2, y_pos2, z_pos2, normal;
	float grad_x, grad_y, grad_z;

	vec4 final_color = vec4(0.0), sample_color;

	vec3 light_dir, refl, view_dir;

	for (int i = 0; i < 1000000; i++) {

		d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
		sample_color = vec4(d,d,d,d);

        sample_color.rgb *= sample_color.a;

		if ((sample_color.a >= u_threshold)  && (sample_color.a <= 1.0)) {

			x_pos1 = vec3(sample_pos.x+u_h, sample_pos.y, sample_pos.z);	// (x+h,y,z)
			x_pos2 = vec3(sample_pos.x-u_h, sample_pos.y, sample_pos.z);	// (x-h,y,z)
			y_pos1 = vec3(sample_pos.x, sample_pos.y+u_h, sample_pos.z);	// (x,y+h,z)
			y_pos2 = vec3(sample_pos.x, sample_pos.y-u_h, sample_pos.z);	// (x,y-h,z)
			z_pos1 = vec3(sample_pos.x, sample_pos.y, sample_pos.z+u_h);	// (x,y,z+h)
			z_pos2 = vec3(sample_pos.x, sample_pos.y, sample_pos.z-u_h);	// (x,y,z-h)

			grad_x = texture3D(u_texture, (x_pos1+1.0)/2.0).x - texture3D(u_texture, (x_pos2+1.0)/2.0).x;	// f(x+h,y,z) - f(x-h,y,z)
			grad_y = texture3D(u_texture, (y_pos1+1.0)/2.0).x - texture3D(u_texture, (y_pos2+1.0)/2.0).x;	// f(x,y+h,z) - (x,y-h,z)
			grad_z = texture3D(u_texture, (z_pos1+1.0)/2.0).x - texture3D(u_texture, (z_pos2+1.0)/2.0).x;	// f(x,y,z+h) - f(x,y,z-h)

			gradient = vec3(grad_x,grad_y,grad_z)/(2*u_h);
			normal = -normalize(gradient);
			light_dir = normalize(u_light_pos-sample_pos);
			view_dir = -dir;
			refl = 2*dot(normal,light_dir)*normal - light_dir;
			final_color = vec4(sample_color.xyz*(dot(light_dir,normal)+pow(dot(refl,view_dir),10.0)),1.0);
			break;
		}
		else if (sample_color.a >= 1.0) {
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
