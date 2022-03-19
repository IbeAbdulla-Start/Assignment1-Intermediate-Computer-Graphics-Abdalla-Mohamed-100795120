#pragma once
#include "Application/ApplicationLayer.h"
#include "json.hpp"
#include <GLM/glm.hpp>
#include "Utils/Macros.h"
#include "Gameplay/Scene.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Material.h"

/**
 * This example layer handles creating a default test scene, which we will use 
 * as an entry point for creating a sample scene
 */
class DefaultSceneLayer final : public ApplicationLayer {
public:
	MAKE_PTRS(DefaultSceneLayer)

	DefaultSceneLayer();
	virtual ~DefaultSceneLayer();

	// Inherited from ApplicationLayer

	virtual void OnAppLoad(const nlohmann::json& config) override;
	void OnUpdate() override;

protected:
	void _CreateScene();
	Gameplay::Scene::Sptr _currentScene;
	bool actcool = false;
	bool actwarm = false;
	bool actcustom = false;

	Texture3D::Sptr coolLut;
	Texture3D::Sptr warmLut;
	Texture3D::Sptr customLut;


};

