#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"
#include "Gameplay/Components/Movement.h"
#include "Gameplay/Components/EnemyMoving.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"
#include "RenderLayer.h"
//int ammo;
//float playerX, playerY;
// float BulletX, BulletY, BulletZ;
//bool Shooting;

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad | AppLayerFunctions::OnUpdate;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}

void DefaultSceneLayer::OnUpdate() {
	Application& app = Application::Get();
	_currentScene = app.CurrentScene();
	RenderLayer::Sptr _renderLayer = app.GetLayer<RenderLayer>();
	RenderFlags _RenderFlags = _renderLayer->GetRenderFlags();
	bool _color = *(_RenderFlags & RenderFlags::EnableColorCorrection);

	


	if ((glfwGetKey(app.GetWindow(), GLFW_KEY_8)) && GLFW_PRESS) {
		
		if (actwarm) {
			
			_color = false;
			_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
			_renderLayer->SetRenderFlags(_RenderFlags);
			actwarm = false;
			actcool = false;
			actcustom = false;
			
		}
		else {
			_currentScene->SetColorLUT(warmLut);
			actwarm = true;
			_color = true;
			actcool = false;
			actcustom = false;
			_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
			_renderLayer->SetRenderFlags(_RenderFlags);
			
			
			
		}
		
	}


	if (glfwGetKey(app.GetWindow(), GLFW_KEY_9)) {

		_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
		if (actcool) {
			_currentScene->SetColorLUT(coolLut);
			_color = false;
			_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
			_renderLayer->SetRenderFlags(_RenderFlags);
			actcool = false;
			actwarm = false;
			actcustom = false;
		}
		else {
			actwarm = false;
			actcool = true;
			actcustom = false;
			_color = true;
			_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
			_renderLayer->SetRenderFlags(_RenderFlags);

		}

	}

	if (glfwGetKey(app.GetWindow(), GLFW_KEY_0)) {

		_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
		if (actcustom) {
			_currentScene->SetColorLUT(customLut);
			_color = false;
			_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
			_renderLayer->SetRenderFlags(_RenderFlags);
			actcool = false;
			actwarm = false;
			actcustom = false;
		}
		else {
			actwarm = false;
			actcool = false;
			actcustom = true;
			_color = true;
			_RenderFlags = (_RenderFlags & ~*RenderFlags::EnableColorCorrection) | (_color ? RenderFlags::EnableColorCorrection : RenderFlags::None);
			_renderLayer->SetRenderFlags(_RenderFlags);

		}

	}


	//Bullet::Sptr bullettest;
	//.playerX = specBox->GetPosition().x;
	///playerY = specBox->GetPosition().y;
	 
	
}

void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	Application& app = Application::Get();



	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials 
		ShaderProgram::Sptr reflectiveShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_environment_reflective.glsl" }
		});
		reflectiveShader->SetDebugName("Reflective");

		ShaderProgram::Sptr ErodeShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/custom_vertex.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/custom_frag.glsl" }
		});
		ErodeShader->SetDebugName("Eroded");

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr basicShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_blinn_phong_textured.glsl" }
		});
		basicShader->SetDebugName("Blinn-phong");

		// This shader handles our basic materials without reflections (cause they expensive)
		ShaderProgram::Sptr specShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/textured_specular.glsl" }
		});
		specShader->SetDebugName("Textured-Specular");

		// This shader handles our foliage vertex shader example
		ShaderProgram::Sptr foliageShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/screendoor_transparency.glsl" }
		});
		foliageShader->SetDebugName("Foliage");

		// This shader handles our cel shading example
		ShaderProgram::Sptr toonShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/toon_shading.glsl" }
		});
		toonShader->SetDebugName("Toon Shader");


		



		// This shader handles our displacement mapping example
		ShaderProgram::Sptr displacementShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});
		displacementShader->SetDebugName("Displacement Mapping");

		// This shader handles our tangent space normal mapping
		ShaderProgram::Sptr tangentSpaceMapping = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_tangentspace_normal_maps.glsl" }
		});
		tangentSpaceMapping->SetDebugName("Tangent Space Mapping");

		// This shader handles our multitexturing example
		ShaderProgram::Sptr multiTextureShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
		});
		multiTextureShader->SetDebugName("Multitexturing");

		// Load in the meshes
		MeshResource::Sptr monkeyMesh = ResourceManager::CreateAsset<MeshResource>("Monkey.obj");

		// Load in some textures
		Texture2D::Sptr    boxTexture   = ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png");
		Texture2D::Sptr    boxSpec      = ResourceManager::CreateAsset<Texture2D>("textures/box-specular.png");
		Texture2D::Sptr    monkeyTex    = ResourceManager::CreateAsset<Texture2D>("textures/monkey-uvMap.png");
		Texture2D::Sptr    leafTex      = ResourceManager::CreateAsset<Texture2D>("textures/leaves.png");
		Texture2D::Sptr    floorTex	    = ResourceManager::CreateAsset<Texture2D>("textures/floor.jpg");
		Texture2D::Sptr    cashTex      = ResourceManager::CreateAsset<Texture2D>("textures/cash.png");
		Texture2D::Sptr    shelfTex     = ResourceManager::CreateAsset<Texture2D>("textures/statue.jpg");
		Texture2D::Sptr    sodaTex      = ResourceManager::CreateAsset<Texture2D>("textures/soda.jpg");
		Texture2D::Sptr    wallTex = ResourceManager::CreateAsset<Texture2D>("textures/wall.jpg");

		

		leafTex->SetMinFilter(MinFilter::Nearest);
		leafTex->SetMagFilter(MagFilter::Nearest);


		// Loading in a 1D LUT
		Texture1D::Sptr toonLut = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png"); 
		toonLut->SetWrap(WrapMode::ClampToEdge);

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>(); 

		// Setting up our enviroment map

		// Loading in a color lookup table
		warmLut = ResourceManager::CreateAsset<Texture3D>("luts/Warm.CUBE");
		coolLut = ResourceManager::CreateAsset<Texture3D>("luts/cool.CUBE");
		customLut = ResourceManager::CreateAsset<Texture3D>("luts/custom.CUBE");

		


		// Create our materials
		// This will be our box material, with no environment reflections
		Material::Sptr boxMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			boxMaterial->Name = "Box";
			boxMaterial->Set("u_Material.Diffuse", boxTexture);
			boxMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr sodaMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			sodaMaterial->Name = "Book";
			sodaMaterial->Set("u_Material.Diffuse", sodaTex);
			sodaMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr cashMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			cashMaterial->Name = "Cash Reg";
			cashMaterial->Set("u_Material.Diffuse", cashTex);
			cashMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr shelfMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			shelfMaterial->Name = "Shelf";
			shelfMaterial->Set("u_Material.Diffuse", shelfTex);
			shelfMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr floorMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			floorMaterial->Name = "Floor";
			floorMaterial->Set("u_Material.Diffuse", floorTex);
			floorMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr wallMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			wallMaterial->Name = "Wall";
			wallMaterial->Set("u_Material.Diffuse", wallTex);
			wallMaterial->Set("u_Material.Shininess", 0.1f);
		}

		// This will be the reflective material, we'll make the whole thing 90% reflective
		Material::Sptr monkeyMaterial = ResourceManager::CreateAsset<Material>(reflectiveShader);
		{
			monkeyMaterial->Name = "Monkey";
			monkeyMaterial->Set("u_Material.Diffuse", monkeyTex);
			monkeyMaterial->Set("u_Material.Shininess", 0.5f);
		}

		// This will be the reflective material, we'll make the whole thing 90% reflective
		Material::Sptr testMaterial = ResourceManager::CreateAsset<Material>(specShader);
		{
			testMaterial->Name = "Box-Specular";
			testMaterial->Set("u_Material.Diffuse", boxTexture);
			testMaterial->Set("u_Material.Specular", boxSpec);
		}

		// Our foliage vertex shader material
		Material::Sptr foliageMaterial = ResourceManager::CreateAsset<Material>(foliageShader);
		{
			foliageMaterial->Name = "Foliage Shader";
			foliageMaterial->Set("u_Material.Diffuse", leafTex);
			foliageMaterial->Set("u_Material.Shininess", 0.1f);
			foliageMaterial->Set("u_Material.Threshold", 0.1f);

			foliageMaterial->Set("u_WindDirection", glm::vec3(1.0f, 1.0f, 0.0f));
			foliageMaterial->Set("u_WindStrength", 0.5f);
			foliageMaterial->Set("u_VerticalScale", 1.0f);
			foliageMaterial->Set("u_WindSpeed", 1.0f);
		}

		// Our toon shader material
		Material::Sptr toonMaterial = ResourceManager::CreateAsset<Material>(toonShader);
		{
			toonMaterial->Name = "Toon";
			toonMaterial->Set("u_Material.Diffuse", boxTexture);
			toonMaterial->Set("s_ToonTerm", toonLut);
			toonMaterial->Set("u_Material.Shininess", 0.1f);
			toonMaterial->Set("u_Material.Steps", 8);
		}

		//Erode material
		Material::Sptr ErodeMaterial = ResourceManager::CreateAsset<Material>(ErodeShader);
		{
			ErodeMaterial->Name = "Erode";
		}

		Material::Sptr displacementTest = ResourceManager::CreateAsset<Material>(displacementShader);
		{
			Texture2D::Sptr displacementMap = ResourceManager::CreateAsset<Texture2D>("textures/displacement_map.png");
			Texture2D::Sptr normalMap       = ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png");
			Texture2D::Sptr diffuseMap      = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");

			displacementTest->Name = "Displacement Map";
			displacementTest->Set("u_Material.Diffuse", diffuseMap);
			displacementTest->Set("s_Heightmap", displacementMap);
			displacementTest->Set("s_NormalMap", normalMap);
			displacementTest->Set("u_Material.Shininess", 0.5f);
			displacementTest->Set("u_Scale", 0.1f);
		}

		Material::Sptr normalmapMat = ResourceManager::CreateAsset<Material>(tangentSpaceMapping);
		{
			Texture2D::Sptr normalMap       = ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png");
			Texture2D::Sptr diffuseMap      = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");

			normalmapMat->Name = "Tangent Space Normal Map";
			normalmapMat->Set("u_Material.Diffuse", diffuseMap);
			normalmapMat->Set("s_NormalMap", normalMap);
			normalmapMat->Set("u_Material.Shininess", 0.5f);
			normalmapMat->Set("u_Scale", 0.1f);
		}

		Material::Sptr multiTextureMat = ResourceManager::CreateAsset<Material>(multiTextureShader);
		{
			Texture2D::Sptr sand  = ResourceManager::CreateAsset<Texture2D>("textures/terrain/sand.png");
			Texture2D::Sptr grass = ResourceManager::CreateAsset<Texture2D>("textures/terrain/grass.png");

			multiTextureMat->Name = "Multitexturing";
			multiTextureMat->Set("u_Material.DiffuseA", sand);
			multiTextureMat->Set("u_Material.DiffuseB", grass);
			multiTextureMat->Set("u_Material.Shininess", 0.5f);
			multiTextureMat->Set("u_Scale", 0.1f);
		}
		//Wolf statue object
		Gameplay::MeshResource::Sptr statueMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("wolfstatue.obj");
		Texture2D::Sptr statueTex = ResourceManager::CreateAsset <Texture2D>("textures/statue.jpg");
		//Create Material

		Material::Sptr statueMaterial = ResourceManager::CreateAsset<Material>(basicShader);
		{
			statueMaterial->Name = "Statuee";
			statueMaterial->Set("u_Material.Diffuse", statueTex);
			statueMaterial->Set("u_Material.Shininess", 0.1f);
		}
		Gameplay::MeshResource::Sptr shelfMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("shelf.obj");
		Gameplay::MeshResource::Sptr cashMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("cashcounter.obj");
		Gameplay::MeshResource::Sptr sodaMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("soda.obj");
		Gameplay::MeshResource::Sptr wallMesh = ResourceManager::CreateAsset<Gameplay::MeshResource>("Wall.obj");


		// Create some lights for our scene
		scene->Lights.resize(4);
		scene->Lights[0].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 20.0f;

		
		scene->Lights[2].Position = glm::vec3(-5.0f, -5.0f, 3.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[2].Range = 10.0f;

		scene->Lights[3].Position = glm::vec3(5.0f, -5.0f, 3.0f);
		scene->Lights[3].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[3].Range = 10.0f;

		//scene->Lights[3].Position = bullet->GetPosition();

		// We'll create a mesh that is a simple plane that we can resize later
		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		MeshResource::Sptr sphere = ResourceManager::CreateAsset<MeshResource>();
		sphere->AddParam(MeshBuilderParam::CreateIcoSphere(ZERO, ONE, 5));
		sphere->GenerateMesh();

		// Set up the scene's camera
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion({ -0.45, -10.2, 2.249 });
			camera->SetRotation({ 85.21f,0.f,0.0f });
			camera->Add<SimpleCameraControl>();

			///* This is now handled by scene itself!*/
			//Camera::Sptr cam = camera->Add<Camera>();
			///* Make sure that the camera is set as the scene's main camera!*/
			//scene->MainCamera = cam;
		}

		// Set up all our sample objects
		GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(floorMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
		}

		Gameplay::GameObject::Sptr layoutwall1 = scene->CreateGameObject("Layout Wall Front");
		{

			layoutwall1->SetPostion(glm::vec3(0.03f, 6.27f, 0.0f));
			layoutwall1->SetScale(glm::vec3(1.19f, 0.97f, 1.64f));
			layoutwall1->SetRotation(glm::vec3(0.0f, -90.f, 90.f));
			RenderComponent::Sptr renderer = layoutwall1->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);
			Gameplay::Physics::RigidBody::Sptr wall1Phys = layoutwall1->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr wall1 = Gameplay::Physics::BoxCollider::Create();
			//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
			wall1->SetExtents(glm::vec3(15.f, 100.0f, 1.0f));
			wall1Phys->AddCollider(wall1);
		}

		Gameplay::GameObject::Sptr layoutwall2 = scene->CreateGameObject("Layout Wall Left");
		{

			layoutwall2->SetPostion(glm::vec3(-14.03f, 6.49f, 0.0f));
			layoutwall2->SetScale(glm::vec3(1.19f, 0.97f, 1.64f));
			layoutwall2->SetRotation(glm::vec3(-170.f, -83.f, 10.f));
			RenderComponent::Sptr renderer = layoutwall2->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);
			Gameplay::Physics::RigidBody::Sptr wall2Phys = layoutwall2->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr wall2 = Gameplay::Physics::BoxCollider::Create();
			//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
			wall2->SetExtents(glm::vec3(15.f, 100.0f, 1.0f));
			wall2Phys->AddCollider(wall2);
		}

		Gameplay::GameObject::Sptr layoutwall3 = scene->CreateGameObject("Layout Wall Right");
		{

			layoutwall3->SetPostion(glm::vec3(12.56f, 6.23f, 0.0f));
			layoutwall3->SetScale(glm::vec3(1.19f, 0.97f, 1.64f));
			layoutwall3->SetRotation(glm::vec3(-30.f, -94.f, 10.f));
			RenderComponent::Sptr renderer = layoutwall3->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);
			Gameplay::Physics::RigidBody::Sptr wall3Phys = layoutwall3->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr wall3 = Gameplay::Physics::BoxCollider::Create();
			//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
			wall3->SetExtents(glm::vec3(15.f, 100.0f, 1.0f));
			wall3Phys->AddCollider(wall3);
		}
		Gameplay::GameObject::Sptr layoutwall4 = scene->CreateGameObject("Layout Wall Right");
		{

			layoutwall4->SetPostion(glm::vec3(0.f, -10.67f, 0.0f));
			layoutwall4->SetScale(glm::vec3(1.19f, 0.97f, 1.64f));
			layoutwall4->SetRotation(glm::vec3(-90.f, 0.f, 0.f));
			RenderComponent::Sptr renderer = layoutwall4->Add<RenderComponent>();
			renderer->SetMesh(wallMesh);
			renderer->SetMaterial(wallMaterial);
			Gameplay::Physics::RigidBody::Sptr wall4Phys = layoutwall4->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr wall4 = Gameplay::Physics::BoxCollider::Create();
			//wall1->SetPosition(glm::vec3(11.85f, 3.23f, 1.05f));
			wall4->SetExtents(glm::vec3(15.f, 100.0f, 1.0f));
			wall4Phys->AddCollider(wall4);
		}
		
		Gameplay::GameObject::Sptr wolfstatue = scene->CreateGameObject("Wolf Statue");
		{
			wolfstatue->SetPostion(glm::vec3(-0.31f, -5.31f, 0.4f));
			wolfstatue->SetRotation(glm::vec3(90.0, 0.0f, 180.f));
			wolfstatue->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = wolfstatue->Add<RenderComponent>();
			renderer->SetMesh(statueMesh);
			renderer->SetMaterial(statueMaterial);
			Movement::Sptr movement = wolfstatue->Add<Movement>();
			Gameplay::Physics::RigidBody::Sptr physics = wolfstatue->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Dynamic);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();

			box->SetExtents(glm::vec3(0.5, 0.5, 0.5));
			//physics->AddCollider(box);
			physics->AddCollider(box);
			physics->SetMass(3.0f);
			//add trigger for collisions and behaviours
			Gameplay::Physics::TriggerVolume::Sptr volume = wolfstatue->Add<Gameplay::Physics::TriggerVolume>();
			volume->AddCollider(BoxCollider::Create());
		}

		Gameplay::GameObject::Sptr wolfstatuecash = scene->CreateGameObject("Wolf Statuecash");
		{
			wolfstatuecash->SetPostion(glm::vec3(-4.200f, 2.74f, 0.4f));
			wolfstatuecash->SetRotation(glm::vec3(90.0, 0.0f, 0.f));
			wolfstatuecash->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

			RenderComponent::Sptr renderer = wolfstatuecash->Add<RenderComponent>();
			renderer->SetMesh(statueMesh);
			renderer->SetMaterial(statueMaterial);
			Gameplay::Physics::RigidBody::Sptr physics = wolfstatuecash->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			
		}

		Gameplay::GameObject::Sptr shelf = scene->CreateGameObject("Shelf");
		{
			shelf->SetPostion(glm::vec3(-4.17f, 4.40f, 0.04f));
			shelf->SetRotation(glm::vec3(90.0, 0.0f, 90.f));
			shelf->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

			RenderComponent::Sptr renderer = shelf->Add<RenderComponent>();
			renderer->SetMesh(shelfMesh);
			renderer->SetMaterial(shelfMaterial);
			Gameplay::Physics::RigidBody::Sptr physics = shelf->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();

			box->SetExtents(glm::vec3(3.0f, 3.0f, 3.0f));
			//physics->AddCollider(box);
			physics->AddCollider(box);
			//add trigger for collisions and behaviours
			Gameplay::Physics::TriggerVolume::Sptr volume = shelf->Add<Gameplay::Physics::TriggerVolume>();
			volume->AddCollider(BoxCollider::Create());
		}
		
		Gameplay::GameObject::Sptr cash = scene->CreateGameObject("cash");
		{
			cash->SetPostion(glm::vec3(-4.46f, 0.98f, 0.04f));
			cash->SetRotation(glm::vec3(90.0, 0.0f, 90.f));
			cash->SetScale(glm::vec3(0.70f, 0.70f, 0.70f));

			RenderComponent::Sptr renderer = cash->Add<RenderComponent>();
			renderer->SetMesh(cashMesh);
			renderer->SetMaterial(cashMaterial);
			Gameplay::Physics::RigidBody::Sptr physics = cash->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			Gameplay::Physics::BoxCollider::Sptr box = Gameplay::Physics::BoxCollider::Create();

			box->SetExtents(glm::vec3(1.00, 1.00, 1.50));
			//physics->AddCollider(box);
			physics->AddCollider(box);
			//add trigger for collisions and behaviours
			Gameplay::Physics::TriggerVolume::Sptr volume = cash->Add<Gameplay::Physics::TriggerVolume>();
			volume->AddCollider(BoxCollider::Create());
		}

		Gameplay::GameObject::Sptr soda = scene->CreateGameObject("soda");
		{
			soda->SetPostion(glm::vec3(-3.11f, 1.31f, -0.888f));
			soda->SetRotation(glm::vec3(90.0, -10.0f, 90.f));
			soda->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

			RenderComponent::Sptr renderer = soda->Add<RenderComponent>();
			renderer->SetMesh(sodaMesh);
			renderer->SetMaterial(sodaMaterial);
			Gameplay::Physics::RigidBody::Sptr physics = soda->Add<Gameplay::Physics::RigidBody>(RigidBodyType::Static);
			//add trigger for collisions and behaviours
			Gameplay::Physics::TriggerVolume::Sptr volume = soda->Add<Gameplay::Physics::TriggerVolume>();
			volume->AddCollider(BoxCollider::Create());
		}

		// Create a trigger volume for testing how we can detect collisions with objects!
		GameObject::Sptr trigger = scene->CreateGameObject("Trigger");
		{
			TriggerVolume::Sptr volume = trigger->Add<TriggerVolume>();
			CylinderCollider::Sptr collider = CylinderCollider::Create(glm::vec3(3.0f, 3.0f, 1.0f));
			collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			volume->AddCollider(collider);

			trigger->Add<TriggerVolumeEnterBehaviour>();
		}

		/////////////////////////// UI //////////////////////////////
		/*
		GameObject::Sptr canvas = scene->CreateGameObject("UI Canvas");
		{
			RectTransform::Sptr transform = canvas->Add<RectTransform>();
			transform->SetMin({ 16, 16 });
			transform->SetMax({ 256, 256 });

			GuiPanel::Sptr canPanel = canvas->Add<GuiPanel>();


			GameObject::Sptr subPanel = scene->CreateGameObject("Sub Item");
			{
				RectTransform::Sptr transform = subPanel->Add<RectTransform>();
				transform->SetMin({ 10, 10 });
				transform->SetMax({ 128, 128 });

				GuiPanel::Sptr panel = subPanel->Add<GuiPanel>();
				panel->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/upArrow.png"));

				Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
				font->Bake();

				GuiText::Sptr text = subPanel->Add<GuiText>();
				text->SetText("Hello world!");
				text->SetFont(font);

				monkey1->Get<JumpBehaviour>()->Panel = text;
			}

			canvas->AddChild(subPanel);
		}
		*/

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
