uniform vec3 u_camera_position;
uniform sampler3D u_texture;
uniform mat4 u_model;
uniform float u_step_length;
uniform float u_brightness;
uniform vec4 u_color;
uniform sampler2D u_noise_txt;
uniform sampler2D u_transfer_function1;
uniform sampler2D u_transfer_function2;
uniform bool u_jittering_text;
uniform bool u_jittering_rand;
uniform vec4 u_clipping_plane;
uniform bool u_clipping1;
uniform bool u_clipping2;
uniform bool u_transfer_bool;
uniform bool u_transfer_bool2;
uniform float u_threshold;

varying vec3 v_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

// Funció pseudo-random per Jittering
float rand(vec2 co) {
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {

    float d, offset;
	vec2 noise_coords = gl_FragCoord.xy/128.0;	// Coordenades de la textura de blue noise.

	if (u_jittering_text) {
		offset = texture2D(u_noise_txt, noise_coords).z;	// Llegim el valor de la textura a les coordenades calculades i agafem el tercer component
															// perquè és blue noise.
	} else if (u_jittering_rand) {
		offset = rand(gl_FragCoord.xy);						// Si seleccionem l'altre mètode, apliquem la funció prèviament definida

	} else {
		offset = 0;											// Si cap de les dues està seleccionada, assignem offset = 0, que és equivalent a no
															// aplicar Jittering
	}

	vec3 dir = normalize(v_position.xyz - u_camera_position.xyz);	// Direcció del step vector normalitzada

	vec3 sample_pos = v_position.xyz + offset*dir;			// Movem la primera sample position en base a l'offset (si l'offset és 0, no es mou)

	vec3 step_vec = dir*u_step_length;						// Step vector és la direcció normalitzada per la longitud, seleccionada interactivament

	vec4 final_color = vec4(0.0), sample_color;				// Inicialitzem alguns valors
	float value;


	if (u_transfer_bool) {		// Posem la condició fora del loop per només comprovar-ho una vegada
								// Representa si es fan servir les respectives transfer functions

		for (int i = 0; i < 1000000; i++) {

			if (u_clipping1 || u_clipping2) {				// Si algun dels modes de clipping està activat
				value = dot(u_clipping_plane.xyz, sample_pos.xyz) + u_clipping_plane.a;	
				if (((u_clipping1) && (value < 0 )) || ((u_clipping2) && (value > 0 ))) {		// Condicions per no representar aquestes posicions
					sample_pos += step_vec;
					if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {	// Comprovem aquí també si hem sortit del volum
						break;
					}
					continue;
				}
			}

			d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
			
			if (d < u_threshold) {		// Condició per estudiar els valors de d

				sample_color = texture2D(u_transfer_function1, vec2(d,1));

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
	} 
	else if (u_transfer_bool2) {		// Condició per utilitzar la transfer function 2
		
		for (int i = 0; i < 1000000; i++) {

			if (u_clipping1 || u_clipping2) {				// Si algun dels modes de clipping està activat
				value = dot(u_clipping_plane.xyz, sample_pos.xyz) + u_clipping_plane.a;	
				if (((u_clipping1) && (value < 0 )) || ((u_clipping2) && (value > 0 ))) {		// Condicions per no representar aquestes posicions
					sample_pos += step_vec;
					if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {	// Comprovem aquí també si hem sortit del volum
						break;
					}
					continue;
				}
			}

			d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
			
			if (d < u_threshold) {		// Condició per estudiar els valors de d

				sample_color = texture2D(u_transfer_function2, vec2(d,1));

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
	}
	else {			// Sense transfer function
		for (int i = 0; i < 1000000; i++) {

			if (u_clipping1 || u_clipping2) {				// Si algun dels modes de clipping està activat
				value = dot(u_clipping_plane.xyz, sample_pos.xyz) + u_clipping_plane.a;	
				if (((u_clipping1) && (value < 0 )) || ((u_clipping2) && (value > 0 ))) {		// Condicions per no representar aquestes posicions
					sample_pos += step_vec;
					if ((abs(sample_pos.x)>1.0) || (abs(sample_pos.y)>1.0) || (abs(sample_pos.z)>1.0)) {	// Comprovem aquí també si hem sortit del volum
						break;
					}
					continue;
				}
			}

			d = texture3D(u_texture, (sample_pos+1.0)/2.0).x;
			
			if (d < u_threshold) {

				sample_color = vec4(d,d,d,d);	// Si no tenim transfer functions activades, el color ve donat pel valor de la densitat de la textura

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

	}
	

	if (final_color.a < 0.03) {
		discard;
	}

	gl_FragColor = u_brightness*final_color*u_color;
}
