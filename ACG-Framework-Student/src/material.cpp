#include "material.h"
#include "texture.h"
#include "application.h"
#include "extra/hdre.h"

StandardMaterial::StandardMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/volume.fs");
}

StandardMaterial::~StandardMaterial()
{

}

void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	Matrix44 invModel = model;
	invModel.inverse();
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", (invModel*Vector4(camera->eye, 1.0)).xyz);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);
	shader->setUniform("u_color", color);

	if (texture)
		shader->setUniform("u_texture", texture);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/volume.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
VolumeMaterial::VolumeMaterial() {
	color = vec4(1.f, 1.f, 1.f, 1.f);
	step_length = 0.02;
	brightness = 10.0;
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/volume.fs");
	texture = NULL;
	jit_idx = 0;
	jit_rand = false;
	jit_text = false;
	noise = NULL;
	transfer = false;
	transfer_function = NULL;
	clipping1 = false;
	clipping2 = false;
	clip_plane[0] = 0.0;
	clip_plane[1] = 0.0;
	clip_plane[2] = 0.0;
	clip_plane[3] = 0.0;
	d_threshold = 1.01;
}
VolumeMaterial::VolumeMaterial(Shader* shader_, Texture* texture_, Texture* noise_, Texture* transfer_function_) {

	color = vec4(1.f, 1.f, 1.f, 1.f);
	step_length = 0.02;
	brightness = 10.0;
	shader = shader_;
	texture = texture_;
	jit_idx = 0;
	jit_rand = false;
	jit_text = false;
	noise = noise_;
	transfer = false;
	transfer_function = transfer_function_;
	clipping1 = false;
	clipping2 = false;
	clip_plane[0] = 0.0;
	clip_plane[1] = 0.0;
	clip_plane[2] = 0.0;
	clip_plane[3] = 0.0;
	d_threshold = 1.01;

}

VolumeMaterial::~VolumeMaterial() {

}

void VolumeMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	Matrix44 invModel = model;
	invModel.inverse();
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", (invModel * Vector4(camera->eye, 1.0)).xyz);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);
	shader->setUniform("u_color", color);
	shader->setUniform("u_step_length", step_length);
	shader->setUniform("u_brightness", brightness);
	shader->setUniform("u_noise_txt", noise);
	shader->setUniform("u_jittering_text", jit_text);
	shader->setUniform("u_jittering_rand", jit_rand);
	shader->setUniform("u_transfer_function", transfer_function);
	shader->setUniform("u_clipping1", clipping1);
	shader->setUniform("u_clipping2", clipping2);
	vec4 clipping_plane = vec4(clip_plane[0], clip_plane[1], clip_plane[2], clip_plane[3]);
	shader->setUniform("u_clipping_plane", clipping_plane);
	shader->setUniform("u_transfer_bool", transfer);
	shader->setUniform("u_threshold", d_threshold);


	if (texture) {
		shader->setUniform("u_texture", texture);
	}
}

void VolumeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		glEnable(GL_BLEND);
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();

		glDisable(GL_BLEND);
	}
}

void VolumeMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	ImGui::SliderFloat("Step Length", &step_length, 0.01, 0.2);
	ImGui::SliderFloat("Brightness", &brightness, 0.0, 50.0);
	ImGui::SliderFloat("Density threshold", &d_threshold, 0.01, 1.01);
	if (ImGui::Combo("Jittering Mode", &jit_idx, "No jittering\0Jittering with texture\0Jittering with function\0")) {
		if (jit_idx == 0) {
			jit_rand = false;
			jit_text = false;
		}
		else if (jit_idx == 1) {
			jit_rand = false;
			jit_text = true;
		}
		else if (jit_idx == 2) {
			jit_rand = true;
			jit_text = false;
		}
	}
	
	ImGui::Checkbox("Clipping", &clipping1);
	if (clipping1) {
		clipping2 = false;
	}
	ImGui::Checkbox("Opposite clipping", &clipping2);

	if (clipping2) {
		clipping1 = false;
	}
	ImGui::SliderFloat4("Clippling plane", clip_plane, -20.0, 20.0);
	ImGui::Checkbox("Transfer function", &transfer);
}





IsosurfaceMaterial::IsosurfaceMaterial(Shader* shader_, Texture* texture_) {

	color = vec4(1.f, 1.f, 1.f, 1.f);
	step_length = 0.02;
	brightness = 10.0;
	shader = shader_;
	texture = texture_;
	d_threshold = 0.3;
	h = 0.001;
	light_pos[0] = 1.0;
	light_pos[1] = 1.0;
	light_pos[2] = 1.0;
}

IsosurfaceMaterial::~IsosurfaceMaterial() {

}

void IsosurfaceMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	Matrix44 invModel = model;
	invModel.inverse();
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", (invModel * Vector4(camera->eye, 1.0)).xyz);
	shader->setUniform("u_model", model);
	shader->setUniform("u_time", Application::instance->time);
	shader->setUniform("u_color", color);
	shader->setUniform("u_step_length", step_length);
	shader->setUniform("u_brightness", brightness);
	shader->setUniform("u_threshold", d_threshold);
	vec3 u_light_pos = vec3(light_pos[0], light_pos[1], light_pos[2]);
	shader->setUniform("u_light_pos", u_light_pos);
	shader->setUniform("u_h", h);
	if (texture) {
		shader->setUniform("u_texture", texture);
	}
}

void IsosurfaceMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	ImGui::SliderFloat("Step Length", &step_length, 0.01, 0.2);
	ImGui::SliderFloat("Brightness", &brightness, 0.0, 50.0);
	ImGui::SliderFloat("Density threshold", &d_threshold, 0.01, 1.01);
	ImGui::DragFloat3("Light position", light_pos, 0.1f);

}