#include "stdafx.h"
#include "Attack.h"
#include "GameObjectContainer.h"
#include "GameWorld.h"

void Attack::Update()
{
	// プレイヤーの位置に攻撃範囲を同期
	SetPosition(m_player->GetPosition());
	
	// アニメーション時間を更新
	m_animationTime = m_active_time.sF();

	// 当たり判定(プレイヤー周囲の円形範囲)
	GameObjectContainer container = GetOwner()->GetGameObjects(GetCollision());

	// 当たっている敵にダメージを与える
	for (size_t i = 0; i < container.GetCount(); i++)
	{
		GameObject* obj = container.GetGameObject(i);
		if (obj && obj->GetTag() == "Enemy")
		{
			obj->Damage(m_attackPower);
		}
	}

	// 寿命管理
	if (m_animationTime >= GetDuration()) {
		Destroy();
	}
}

void Attack::Draw() const
{
	if (!m_player) return;

	switch (m_attackType)
	{
	case AttackType::NORMAL:
		DrawNormalEffect();
		break;
	case AttackType::WHIRLWIND:
		DrawWhirlwindEffect();
		break;
	case AttackType::SHOCKWAVE:
		DrawShockwaveEffect();
		break;
	case AttackType::LIGHTNING:
		DrawLightningEffect();
		break;
	case AttackType::FIRE_BLAST:
		DrawFireBlastEffect();
		break;
	}
}

void Attack::DrawNormalEffect() const
{
	double progress = m_animationTime / GetDuration();  // 0.0 -> 1.0
	
	// 攻撃範囲の可視化（エフェクト）
	Vec2 playerPos = GetPosition();
	
	// 拡散する円エフェクト
	double currentRadius = NORMAL_RADIUS * progress;
	double alpha = 1.0 - progress;  // 時間とともにフェードアウト
	
	// 外側の衝撃波
	ColorF shockwaveColor = ColorF(0.8, 0.3, 1.0, alpha * 0.8);
	Circle(playerPos, currentRadius).drawFrame(4, shockwaveColor);
	
	// 内側の強い光
	ColorF innerColor = ColorF(1.0, 0.8, 0.9, alpha * 0.6);
	Circle(playerPos, currentRadius * 0.6).drawFrame(6, innerColor);
	
	// 中心部の爆発エフェクト
	ColorF coreColor = ColorF(1.0, 1.0, 1.0, alpha * 0.9);
	Circle(playerPos, currentRadius * 0.3).draw(coreColor);
	
	// 回転する星形パーティクル
	for (int i = 0; i < 8; ++i)
	{
		double angle = m_animationTime * 10.0 + i * Math::TwoPi / 8;
		double particleRadius = currentRadius * 0.8;
		Vec2 particlePos = playerPos + Vec2(
			Math::Cos(angle) * particleRadius,
			Math::Sin(angle) * particleRadius
		);
		
		ColorF particleColor = ColorF(0.9, 0.6, 1.0, alpha * 0.7);
		Circle(particlePos, 3).draw(particleColor);
	}
}

void Attack::DrawWhirlwindEffect() const
{
	Vec2 playerPos = GetPosition();
	double progress = m_animationTime / GetDuration();
	double alpha = 1.0 - progress;
	
	// 回転する風のエフェクト
	for (int layer = 0; layer < 3; ++layer)
	{
		double radius = SPECIAL_RADIUS * (0.3 + layer * 0.35);
		double rotationSpeed = 15.0 - layer * 3.0;
		
		for (int i = 0; i < 12; ++i)
		{
			double angle = m_animationTime * rotationSpeed + i * Math::TwoPi / 12;
			Vec2 windPos = playerPos + Vec2(
				Math::Cos(angle) * radius,
				Math::Sin(angle) * radius
			);
			
			ColorF windColor = ColorF(0.7, 1.0, 0.8, alpha * (0.8 - layer * 0.2));
			Circle(windPos, 4 - layer).draw(windColor);
		}
	}
	
	// 中央の竜巻エフェクト
	ColorF coreColor = ColorF(0.5, 0.9, 0.6, alpha * 0.6);
	Circle(playerPos, SPECIAL_RADIUS * 0.2).drawFrame(3, coreColor);
}

void Attack::DrawShockwaveEffect() const
{
	Vec2 playerPos = GetPosition();
	double progress = m_animationTime / GetDuration();
	
	// 拡散する衝撃波
	double currentRadius = SPECIAL_RADIUS * progress;
	double alpha = 1.0 - progress;
	
	// 複数の波
	for (int wave = 0; wave < 3; ++wave)
	{
		double waveDelay = wave * 0.1;
		double waveProgress = Max(0.0, (m_animationTime - waveDelay) / GetDuration());
		double waveRadius = SPECIAL_RADIUS * waveProgress;
		double waveAlpha = (1.0 - waveProgress) * alpha;
		
		if (waveProgress > 0.0)
		{
			ColorF waveColor = ColorF(0.9, 0.7, 0.3, waveAlpha * 0.7);
			Circle(playerPos, waveRadius).drawFrame(4 - wave, waveColor);
		}
	}
	
	// 地面の亀裂エフェクト
	for (int i = 0; i < 8; ++i)
	{
		double angle = i * Math::TwoPi / 8;
		Vec2 crackEnd = playerPos + Vec2(
			Math::Cos(angle) * currentRadius,
			Math::Sin(angle) * currentRadius
		);
		
		ColorF crackColor = ColorF(0.8, 0.4, 0.1, alpha * 0.8);
		Line(playerPos, crackEnd).draw(3, crackColor);
	}
}

void Attack::DrawLightningEffect() const
{
	Vec2 playerPos = GetPosition();
	double progress = m_animationTime / GetDuration();
	double alpha = 1.0 - progress;
	
	// 雷のエフェクト
	for (int bolt = 0; bolt < 6; ++bolt)
	{
		double angle = bolt * Math::TwoPi / 6 + m_animationTime * 5.0;
		Vec2 boltEnd = playerPos + Vec2(
			Math::Cos(angle) * SPECIAL_RADIUS,
			Math::Sin(angle) * SPECIAL_RADIUS
		);
		
		// シンプルな雷の線
		ColorF lightningColor = ColorF(0.9, 0.9, 1.0, alpha);
		Line(playerPos, boltEnd).draw(3, lightningColor);
	}
	
	// 中央の電気球
	ColorF coreColor = ColorF(0.8, 0.8, 1.0, alpha * 0.8);
	double pulseRadius = SPECIAL_RADIUS * 0.3 * (1.0 + Math::Sin(m_animationTime * 20.0) * 0.2);
	Circle(playerPos, pulseRadius).draw(coreColor);
}

void Attack::DrawFireBlastEffect() const
{
	Vec2 playerPos = GetPosition();
	double progress = m_animationTime / GetDuration();
	double alpha = 1.0 - progress;
	
	// 爆発の炎エフェクト
	double currentRadius = SPECIAL_RADIUS * progress;
	
	// 外側の爆炎
	for (int flame = 0; flame < 12; ++flame)
	{
		double angle = flame * Math::TwoPi / 12 + m_animationTime * 3.0;
		double flameRadius = currentRadius * (0.8 + Random(-0.2, 0.2));
		Vec2 flamePos = playerPos + Vec2(
			Math::Cos(angle) * flameRadius,
			Math::Sin(angle) * flameRadius
		);
		
		ColorF flameColor = ColorF(1.0, 0.5, 0.0, alpha * 0.6);
		Circle(flamePos, 8).draw(flameColor);
	}
	
	// 内側の高温部
	ColorF hotColor = ColorF(1.0, 0.9, 0.3, alpha * 0.8);
	Circle(playerPos, currentRadius * 0.6).draw(hotColor);
	
	// 中心の白熱部
	ColorF coreColor = ColorF(1.0, 1.0, 0.9, alpha);
	Circle(playerPos, currentRadius * 0.3).draw(coreColor);
}

double Attack::GetDuration() const
{
	switch (m_attackType)
	{
	case AttackType::NORMAL:    return 0.5;
	case AttackType::WHIRLWIND: return 1.5;
	case AttackType::SHOCKWAVE: return 1.0;
	case AttackType::LIGHTNING: return 0.8;
	case AttackType::FIRE_BLAST: return 1.2;
	default: return 0.5;
	}
}
