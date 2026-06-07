#pragma once
#include <Siv3D.hpp>

enum class SkillType {
	Fireball,      // Q:  火球（直線攻撃）
	IceSpike,      // E: 氷の槍（貫通）
	Teleport,      // R: 瞬間移動
	AreaAttack     // F: 範囲攻撃
};

struct SkillData {
	SkillType type;
	s3d::String name;
	int manaCost;
	double cooldown;
	double damageMultiplier;  // プレイヤー攻撃力への倍率
	s3d::ColorF color;
	s3d::String iconText;  // アイコン用テキスト
};

class Skill {
private:
	SkillData m_data;
	s3d::Stopwatch m_cooldownTimer;

public:
	Skill(SkillType type);

	bool CanUse(int currentMP) const;
	void Use();
	double GetCooldownRatio() const;
	bool IsReady() const;

	const SkillData& GetData() const { return m_data; }
};
