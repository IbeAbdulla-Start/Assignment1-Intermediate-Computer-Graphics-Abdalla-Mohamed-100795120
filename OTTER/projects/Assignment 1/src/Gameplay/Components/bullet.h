#pragma once
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Components/IComponent.h"
class Bullet : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<Bullet> Sptr;

	Bullet();
	virtual ~Bullet();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;

public:
	virtual void RenderImGui() override;


	MAKE_TYPENAME(Bullet);
	virtual nlohmann::json ToJson() const override;
	static Bullet::Sptr FromJson(const nlohmann::json& blob);

protected:
	float _impulse;
	Gameplay::Physics::RigidBody::Sptr _body;
	bool _isPressed = false;
};