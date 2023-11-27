#include "scenenode.h"
#include "application.h"
#include "texture.h"
#include "utils.h"

unsigned int SceneNode::lastNameId = 0;
unsigned int mesh_selected = 0;

SceneNode::SceneNode()
{
	this->name = std::string("Node" + std::to_string(lastNameId++));
}

SceneNode::SceneNode(const char * name)
{
	this->name = name;
}

SceneNode::~SceneNode()
{

}

void SceneNode::render(Camera* camera)
{
	if (material)
		material->render(mesh, model, camera);
}

void SceneNode::renderWireframe(Camera* camera)
{
	WireframeMaterial mat = WireframeMaterial();
	mat.render(mesh, model, camera);
}

void SceneNode::renderInMenu()
{
	//Model edit
	if (ImGui::TreeNode("Model")) 
	{
		ImGui::Checkbox("Autorotate", &Application::instance->autorotate);
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(model.m, matrixTranslation, matrixRotation, matrixScale);
		ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
		ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
		ImGui::DragFloat3("Scale", matrixScale, 0.1f);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.m);
		
		ImGui::TreePop();
	}

	//Material
	if (material && ImGui::TreeNode("Material"))
	{
		material->renderInMenu();
		ImGui::TreePop();
	}
}

VolumeNode::VolumeNode(Shader* shader[2], Texture* texts_[7], Matrix44 mods_[6], Texture* noise_, Texture* transfers_[6], Mesh* aux_mesh_)
{
	mesh = aux_mesh_;
	index = 0;
	isosurf = false;

	for (int i = 0; i < 6; i++) {
		if (i == 3) {		// We have two transfer functions for the abdomen, which we know is index 3
			mats[i] = new VolumeMaterial(shader[0], texts_[i], noise_, transfers_[i], true, transfers_[6]);
		}
		else {
			mats[i] = new VolumeMaterial(shader[0], texts_[i], noise_, transfers_[i], false, NULL);
		}
		mods[i] = mods_[i];

	}
	for (int i = 6; i < 12; i++) {	// We store 12 different materials, the first 6 as VolumeMaterials, and the other 6 as IsosurfaceMaterial
		mats[i] = new IsosurfaceMaterial(shader[1], texts_[i-6]);
	}

	
	material = mats[index];
	model = mods[index];
}

void VolumeNode::renderInMenu()
{
	if (ImGui::TreeNode("Model"))
	{
		ImGui::Checkbox("Autorotate", &Application::instance->autorotate);
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(model.m, matrixTranslation, matrixRotation, matrixScale);
		ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
		ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
		ImGui::DragFloat3("Scale", matrixScale, 0.1f);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.m);

		ImGui::TreePop();
	}

	//Material
	if (material && ImGui::TreeNode("Material"))
	{
		if (ImGui::Combo("Volume", &index, "Foot\0Bonsai\0Teapot\0Abdomen\0Orange\0Daisy\0")) {
			material = mats[index];
			model = mods[index];
		}
		ImGui::Checkbox("Isosurface", &isosurf);
		if (isosurf) {
			material = mats[index+6];		// The material is the same, but as an IsosurfaceMaterial
		}
		else {
			material = mats[index];
		}

		material->renderInMenu();
		ImGui::TreePop();
	}

}
