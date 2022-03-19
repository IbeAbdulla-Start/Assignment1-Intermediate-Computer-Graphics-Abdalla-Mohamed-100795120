#pragma once
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Components/IComponent.h"
class EnemyMoving : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<EnemyMoving> Sptr;

	EnemyMoving();
	virtual ~EnemyMoving();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;


public:
	virtual void RenderImGui() override;


	MAKE_TYPENAME(EnemyMoving);
	virtual nlohmann::json ToJson() const override;
	static EnemyMoving::Sptr FromJson(const nlohmann::json& blob);

protected:
	float _impulse;
};

