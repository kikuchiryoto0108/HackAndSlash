#include "stdafx.h"
#include "Skill.h"

Skill::Skill(SkillType type)
{
	m_cooldownTimer.start();

	switch (type)
	{
	case SkillType::Fireball:
		m_data = {
			type,
			U"ファイアボール",
			20,
			3.0,
			2.0,
			ColorF(1.0, 0.3, 0.2),
			U"🔥"
		};
		break;

	case SkillType::IceSpike:
		m_data = {
			type,
			U"アイススパイク",
			30,
			5.0,
			2.5,
			ColorF(0.3, 0.7, 1.0),
			U"❄️"
		};
		break;

	case SkillType::Teleport:
		m_data = {
			type,
			U"テレポート",
			15,
			8.0,
			0.0,
			ColorF(0.8, 0.3, 1.0),
			U"⚡"
		};
		break;

	case SkillType::AreaAttack:
		m_data = {
			type,
			U"サンダーストーム",
			40,
			10.0,
			3.0,
			ColorF(1.0, 1.0, 0.3),
			U"💥"
		};
		break;
	}
}

bool Skill::CanUse(int currentMP) const
{
	return IsReady() && currentMP >= m_data.manaCost;
}

void Skill::Use()
{
	m_cooldownTimer.restart();
}

double Skill::GetCooldownRatio() const
{
	double elapsed = m_cooldownTimer.sF();
	if (elapsed >= m_data.cooldown) return 1.0;
	return elapsed / m_data.cooldown;
}

bool Skill::IsReady() const
{
	return m_cooldownTimer.sF() >= m_data.cooldown;
}
