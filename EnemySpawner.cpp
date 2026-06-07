#include "stdafx.h"
#include "EnemySpawner.h"
#include "GameWorld.h"
#include "Enemy.h"
#include "GameObjectContainer.h"

void EnemySpawner::Update()
{
	if (m_interval.sF() >= 3.0)
	{
		m_interval.restart();

		GameObjectContainer enemys = GetOwner()->GetGameObjects("Enemy");

		if (enemys.GetCount() < 5) {
			GetOwner()->Registar(new Enemy(GetOwner(), GetPosition()));
		}
	}
}

void EnemySpawner::Draw() const
{
	TextureAsset(U"EnemySpawner").drawAt(GetPosition(), Palette::Red);
}
