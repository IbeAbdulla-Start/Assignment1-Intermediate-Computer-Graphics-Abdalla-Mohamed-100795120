#include "Gameplay/Components/EnemyMoving.h"
#include <GLFW/glfw3.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/InputEngine.h"
#include "Application/Application.h"


EnemyMoving::EnemyMoving(): 
   IComponent(),
_impulse(10.0f)
{
}


EnemyMoving::~EnemyMoving()
{
}

void EnemyMoving::Awake()
{
}

void EnemyMoving::Update(float deltaTime)
{
    glm::vec3 worldLocation = glm::vec3(0, -1, 0);

    worldLocation *= deltaTime;
    GetGameObject()->SetPostion(GetGameObject()->GetPosition() + worldLocation);
}

void EnemyMoving::RenderImGui()
{
}

nlohmann::json EnemyMoving::ToJson() const
{
    return nlohmann::json();
}

EnemyMoving::Sptr EnemyMoving::FromJson(const nlohmann::json& blob)
{
    return EnemyMoving::Sptr();
}
