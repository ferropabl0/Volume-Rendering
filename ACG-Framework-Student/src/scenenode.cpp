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

VolumeNode::VolumeNode(Shader* shader[2], Texture* texts_[6], Matrix44 mods_[6], Texture* noise_, Texture* transfers_[6], Mesh* aux_mesh_, bool isosurf)
{
	mesh = aux_mesh_;
	index = 1;
	if (isosurf) {
		for (int i = 0; i < 6; i++) {
			mats[i] = new IsosurfaceMaterial(shader[1], texts_[i]);
			mods[i] = mods_[i];
		}
	}
	else {
		for (int i = 0; i < 6; i++) {
			mats[i] = new VolumeMaterial(shader[0], texts_[i], noise_, transfers_[3]);
			mods[i] = mods_[i];
		}
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

		material->renderInMenu();
		ImGui::TreePop();
	}
}
