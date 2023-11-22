#ifndef SCENENODE_H
#define SCENENODE_H

#include "framework.h"

#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "material.h"

class Light;

class SceneNode {
public:

	static unsigned int lastNameId;

	SceneNode();
	SceneNode(const char* name);
	~SceneNode();

	Material * material = NULL;
	std::string name;

	Mesh* mesh = NULL;
	Matrix44 model;

	virtual void render(Camera* camera);
	virtual void renderWireframe(Camera* camera);
	virtual void renderInMenu();
};

// TODO: Derived class VolumeNode

class VolumeNode : public SceneNode
{	
	VolumeMaterial* mat0, * mat1, * mat2, * mat3;
	Matrix44 mod0, mod1, mod2, mod3;
	int index;
public:
	VolumeNode(Shader* shader, Texture* text0, Texture* text1, Texture* text2, Texture* text3, Matrix44 mod0_, Matrix44 mod1_, Matrix44 mod2_, Matrix44 mod3_, int index_, Texture* noise_, Texture* transfer_function_);
	void renderInMenu();

};

#endif