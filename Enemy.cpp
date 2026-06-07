#include "stdafx.h"
#include "Enemy.h"
#include "GameObjectContainer.h"
#include "GameWorld.h"
#include "Item.h"
#include "ItemDropper.h"
#include "Player.h"

Enemy::Enemy(GameWorld* owner, const Vec2& position) : GameObject(owner, "Enemy")
{
	SetPosition(position);
	TextureAsset::Register(U"Enemy", 0xf916_icon, 64);
	m_attackCooldown.start();  // ★攻撃クールダウン開始
}

void Enemy::Update()
{
	// 死亡判定
	if (m_hp <= 0) {
		if (!IsDestroy()) {
			// プレイヤーに経験値を付与
			GameObjectContainer players = GetOwner()->GetGameObjects("Player");
			if (players.GetCount() > 0)
			{
				Player* player = dynamic_cast<Player*>(players.GetGameObject(0));
				if (player)
				{
					player->GainExp(m_expReward);
				}
			}

			// アイテムドロップ
			ItemDropper::DropItemsFromEnemy(GetOwner(), GetPosition(), 1);

			for (int i = 0; i < s3d::Random(2, 4); ++i)
			{
				ItemDropper::DropItem(GetOwner(), GetPosition(), 401, 1);
			}

			if (s3d::Random(0.0, 1.0) < 0.3)
			{
				int potionId = s3d::Random(0, 1) == 0 ? 1 : 2;
				ItemDropper::DropItem(GetOwner(), GetPosition(), potionId, 1);
			}
		}
		Destroy();
		return;
	}

	// プレイヤーを取得
	GameObjectContainer container = GetOwner()->GetGameObjects("Player");
	if (!container.GetCount()) return;

	GameObject* playerObj = container.GetGameObject(0);
	Vec2 player_position = playerObj->GetPosition();

	// ★追加: プレイヤーとの距離を計算
	Vec2 to_player = player_position - GetPosition();
	double distanceToPlayer = to_player.length();

	// ★追加: 攻撃範囲内なら攻撃、それ以外は移動
	constexpr double attackRange = 60.0;

	if (distanceToPlayer <= attackRange)
	{
		// 攻撃範囲内 - 攻撃処理
		if (m_attackCooldown.sF() >= m_attackInterval)
		{
			playerObj->Damage(m_attackPower);
			m_attackCooldown.restart();
		}
	}
	else
	{
		// 攻撃範囲外 - プレイヤーに向かって移動
		SetPosition(GetPosition() + to_player.normalized() * MOVE_SPEED * Scene::DeltaTime());
	}
}

void Enemy::Draw() const
{
	TextureAsset(U"Enemy").drawAt(GetPosition(), Palette::Red);
	
	// HPバー表示
	constexpr int barWidth = 60;
	constexpr int barHeight = 8;
	Point barPos(
	static_cast<int>(GetPosition().x - barWidth / 2),
	static_cast<int>(GetPosition().y - 50)
	);
	
	// HPバー背景
	Rect hpBarBg(barPos, barWidth, barHeight);
	hpBarBg.draw(ColorF(0.2, 0.2, 0.2, 0.8));
	hpBarBg.drawFrame(1, ColorF(0.5, 0.5, 0.5));
	
	// HP残量バー
	double hpRatio = static_cast<double>(m_hp) / m_maxHp;
	Rect hpBar(barPos, static_cast<int>(barWidth * hpRatio), barHeight);
	
	// HP割合で色変更
	ColorF hpColor;
	if (hpRatio > 0.6)
		hpColor = ColorF(0.3, 1.0, 0.3);      // 緑
	else if (hpRatio > 0.3)
		hpColor = ColorF(1.0, 0.9, 0.2);     // 黄
	else
		hpColor = ColorF(1.0, 0.3, 0.3);     // 赤
	
	hpBar.draw(hpColor);
	
	// HP数値表示
	static bool fontRegistered = false;
	if (!fontRegistered)
	{
		FontAsset::Register(U"EnemyHP", 11);
		fontRegistered = true;
	}
	s3d::String hpText = U"{}/{}"_fmt(m_hp, m_maxHp);
	FontAsset(U"EnemyHP")(hpText)
		.drawAt(barPos.x + barWidth / 2, barPos.y + barHeight / 2, ColorF(1.0, 1.0, 1.0));
}

