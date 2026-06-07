#include "stdafx.h"
#include "SkillProjectile.h"
#include "GameWorld.h"
#include "GameObjectContainer.h"

SkillProjectile::SkillProjectile(GameWorld* owner, const Vec2& position, const Vec2& direction,
	int damage, SkillType skillType, ColorF color)
	: GameObject(owner, "SkillProjectile")
	, m_direction(direction.normalized())
	, m_speed(400.0)
	, m_damage(damage)
	, m_skillType(skillType)
	, m_color(color)
	, m_lifetime(3.0)
{
	SetPosition(position);
	m_lifeTimer.start();
}

void SkillProjectile::Update()
{
	// 寿命チェック
	if (m_lifeTimer.sF() > m_lifetime)
	{
		Destroy();
		return;
	}

	// 移動
	SetPosition(GetPosition() + m_direction * m_speed * Scene::DeltaTime());

	// 敵との衝突判定
	GameObjectContainer enemies = GetOwner()->GetGameObjects(GetCollision());
	for (size_t i = 0; i < enemies.GetCount(); ++i)
	{
		GameObject* obj = enemies.GetGameObject(i);
		if (obj->GetTag() == "Enemy")
		{
			obj->Damage(m_damage);

			// 貫通しないスキルは消滅
			if (m_skillType != SkillType::IceSpike)
			{
				Destroy();
				break;
			}
		}
	}
}

void SkillProjectile::Draw() const
{
	Circle(GetPosition(), 15).draw(m_color);
	Circle(GetPosition(), 15).drawFrame(2, ColorF(1.0, 1.0, 1.0, 0.5));
}

MyCircle SkillProjectile::GetCollision() const
{
	return { GetPosition(), 15.0 };
}
