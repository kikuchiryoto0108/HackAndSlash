#pragma once
#include "GameObject.h"
#include "Skill.h"

class SkillProjectile : public GameObject
{
private:
	Vec2 m_direction;
	double m_speed;
	int m_damage;
	SkillType m_skillType;
	ColorF m_color;
	double m_lifetime;
	Stopwatch m_lifeTimer;

public:
	SkillProjectile(GameWorld* owner, const Vec2& position, const Vec2& direction,
		int damage, SkillType skillType, ColorF color);

	void Update() override;
	void Draw() const override;
	MyCircle GetCollision() const override;
};
