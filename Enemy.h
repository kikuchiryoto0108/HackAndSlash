#pragma once

#include "GameObject.h"


class Enemy : public GameObject
{
private:

private:
	static constexpr double RADIUS = 32.0;
	static constexpr double MOVE_SPEED = 100.0;
	int m_hp{ 100 };
	int m_maxHp = 100;  // 最大HP
	int m_level = 1;    // 敵のレベル
	int m_expReward = 30;  // 撃破時の経験値

	Stopwatch m_attackCooldown{};
	int m_attackPower = 10;
	double m_attackInterval = 1.0;

	enum class State { NORMAL, ATTACK };
	State m_state{ State::NORMAL };
public:
	Enemy(GameWorld* owner, const Vec2& position);
	void Damage(int damage)override { m_hp -= damage; }
	void Update()override;
	void Draw() const override;

	MyCircle GetCollision()const { return { GetPosition(), RADIUS }; }
	
	int GetMaxHP() const { return m_maxHp; }
	int GetExpReward() const { return m_expReward; }

private:
	void attack();
};
