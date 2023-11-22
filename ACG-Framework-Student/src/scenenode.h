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

	VolumeMaterial* mats[6];
	Matrix44 mods[6];
	int index;

public:
	VolumeNode(Shader* shader, Texture* texts_[6], Matrix44 mods_[6], Texture* noise_, Texture* transfers_[12]);
	void renderInMenu();

};

#endif