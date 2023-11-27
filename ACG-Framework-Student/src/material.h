#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "extra/hdre.h"

class Material {
public:

	Shader* shader = NULL;
	Texture* texture = NULL;
	vec4 color;

	virtual void setUniforms(Camera* camera, Matrix44 model) = 0;
	virtual void render(Mesh* mesh, Matrix44 model, Camera * camera) = 0;
	virtual void renderInMenu() = 0;
};

class StandardMaterial : public Material {
public:

	StandardMaterial();
	~StandardMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class WireframeMaterial : public StandardMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Matrix44 model, Camera* camera);
};

// TODO: Derived class VolumeMaterial

class VolumeMaterial : public StandardMaterial
{
public:
	float step_length, brightness, d_threshold;
	int jit_idx;
	bool jit_text, jit_rand, clipping1, clipping2, transfer, two_transfers;
	float clip_plane[4];
	Texture* noise;
	Texture* transfer_function;
	VolumeMaterial();
	VolumeMaterial(Shader* shader_, Texture* texture_, Texture* noise_, Texture* transfer_function_);
	~VolumeMaterial();
	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera* camera);
	void VolumeMaterial::renderInMenu();

};

class IsosurfaceMaterial : public VolumeMaterial		// Subclass for isosurfaces
{
public:
	float h;					
	float light_pos[3];
	IsosurfaceMaterial(Shader* shader_, Texture* texture_);
	~IsosurfaceMaterial();
	void setUniforms(Camera* camera, Matrix44 model);
	void IsosurfaceMaterial::renderInMenu();
};

#endif