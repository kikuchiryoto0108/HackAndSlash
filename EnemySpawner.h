#pragma once
#include "GameObject.h"

class EnemySpawner : public GameObject
{
private:
	Stopwatch m_interval;
public:
	EnemySpawner(GameWorld* owner, const Vec2& position) :GameObject(owner, "EnemySpawner") {
		SetPosition(position);
		TextureAsset::Register(U"EnemySpawner", 0xf1eb_icon, 64);
		m_interval.start();
	}

	void Update()override;

	void Draw() const override;


};


