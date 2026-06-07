#ifndef ATTACK_H
#define ATTACK_H

#include "GameObject.h"
#include "Collision.h"
#include "Player.h"

/// @brief 攻撃の種類
enum class AttackType
{
	NORMAL,     // 通常攻撃
	WHIRLWIND,  // 旋風斬り
	SHOCKWAVE,  // 衝撃波
	LIGHTNING,  // 雷撃
	FIRE_BLAST  // 炎の爆発
};

class Attack : public GameObject
{
private:
	static constexpr float NORMAL_RADIUS = 80.0f;   // 通常攻撃半径
	static constexpr float SPECIAL_RADIUS = 120.0f; // 特殊攻撃半径
	Stopwatch m_active_time{};
	const Player* m_player{};
	double m_animationTime = 0.0;  // アニメーション用時間
	AttackType m_attackType = AttackType::NORMAL;
	int m_attackPower = 100;
	
public:
	Attack(GameWorld* owner, const Player* player, AttackType type = AttackType::NORMAL, int power = 100) 
		: GameObject(owner, "Attack"), m_player(player), m_attackType(type), m_attackPower(power) {
		SetPosition(m_player->GetPosition());  // プレイヤーと同じ位置に配置
		TextureAsset::Register(U"Attack", 0xf1e2_icon, 32);
		m_active_time.start();
	}

	void Update() override;
	void Draw() const override;
	
	/// @brief 攻撃範囲の当たり判定を取得
	MyCircle GetCollision() const override { 
		float radius = (m_attackType == AttackType::NORMAL) ? NORMAL_RADIUS : SPECIAL_RADIUS;
		return { GetPosition(), radius }; 
	}

private:
	/// @brief 攻撃タイプ別のエフェクト描画
	void DrawNormalEffect() const;
	void DrawWhirlwindEffect() const;
	void DrawShockwaveEffect() const;
	void DrawLightningEffect() const;
	void DrawFireBlastEffect() const;
	
	/// @brief 攻撃タイプ別の持続時間取得
	double GetDuration() const;
};

#endif // ATTACK_H
