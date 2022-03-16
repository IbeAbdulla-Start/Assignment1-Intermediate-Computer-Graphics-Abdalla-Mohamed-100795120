#include "Gameplay/Components/Bullet.h"
#include <GLFW/glfw3.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/InputEngine.h"
#include "Application/Application.h"

void Bullet::Awake()
{
    
}

void Bullet::RenderImGui()
{
}

nlohmann::json Bullet::ToJson() const
{
    return nlohmann::json();
}

Bullet::Sptr Bullet::FromJson(const nlohmann::json& blob)
{
    return Bullet::Sptr();
}



Bullet::Bullet() :
    IComponent(),
    _impulse(10.0f)
{
}

Bullet::~Bullet() = default;


 int ammo;
 float playerX, playerY;
 float BulletX, BulletY;
 bool Shooting;
float fireTime = 0;


extern bool Shooting;


void Bullet::Update(float deltaTime)
{
 
    if (InputEngine::IsKeyDown(GLFW_KEY_SPACE))

        if (fireTime <= 0)
        {
            if (Shooting == false)
            {
                Shooting = true;
                fireTime = 2.f;
            }
        }

    playerY = GetGameObject()->GetScene()->FindObjectByName("Specular Object")->GetPosition().y;
    playerX = GetGameObject()->GetScene()->FindObjectByName("Specular Object")->GetPosition().x;
    if (Shooting == false)
    {

        GetGameObject()->SetScale(glm::vec3(0.01f, 0.01f, 0.01f));
        GetGameObject()->SetPostion(glm::vec3(playerX, playerY, 0.6f));
        fireTime = 0;
        std::cout << playerX << ", " << playerY << "\n";

    }
    else if (Shooting == true)
    {

        glm::vec3 playerLocation = glm::vec3(playerX , -playerY, 0);

        playerLocation *= deltaTime;  

        GetGameObject()->SetPostion(GetGameObject()->GetPosition() + playerLocation);

        GetGameObject()->SetScale(glm::vec3(0.3f));
        std::cout << playerX << ", " << playerY << "\n";
    }

    if (fireTime > 0)
    {
        fireTime -= 1 * deltaTime;
    }
    else
    {
        if (Shooting == true)
            ammo = 1;

        Shooting = false;
    }


}