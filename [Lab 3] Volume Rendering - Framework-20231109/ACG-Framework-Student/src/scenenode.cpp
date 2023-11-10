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

VolumeNode::VolumeNode(Shader* shader, Texture* text0, Texture* text1, Texture* text2, Matrix44 mod0_, Matrix44 mod1_, Matrix44 mod2_, int index_)
{
	Mesh* aux_mesh = new Mesh();
	aux_mesh->createCube();
	mesh = aux_mesh;
	index = index_;
	mat0 = new VolumeMaterial(shader, text0);
	mat1 = new VolumeMaterial(shader, text1);
	mat2 = new VolumeMaterial(shader, text2);
	mod0 = mod0_;
	mod1 = mod1_;
	mod2 = mod2_;

	if (index == 0) {
		material = mat0;
		model = mod0;
	}
	else if (index == 1) {
		material = mat1;
		model = mod1;
	}
	else if (index == 2) {
		material = mat2;
		model = mod2;
	}
}
void VolumeNode::render(Camera* camera)
{
	if (material)
		material->render(mesh, model, camera);
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
		if (ImGui::Combo("Volume", &index, "Foot\0Bonsai\0Orange")) {
			if (index == 0) {
				material = mat0;
				model = mod0;
			}
			else if (index == 1) {
				material = mat1;
				model = mod1;
			}
			else if (index == 2) {
				material = mat2;
				model = mod2;
			}
		}
		material->renderInMenu();
		ImGui::TreePop();
	}
}
