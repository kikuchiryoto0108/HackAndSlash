#pragma once

#include "GameObject.h"
#include "Inventory.h"
#include "ItemMenuUI.h"
#include "Skill.h"

class GameWorld;

class Player : public GameObject
{
private:
	Texture m_texture_normal{ 0xe06c_icon, 128 };
	Texture m_texture_attack{ 0xF17C4_icon, 128 };
	Stopwatch m_delay{};

private:
	static constexpr double MOVE_SPEED = 100.0;
	static constexpr double RADIUS = 64.0;
	int m_hp{ 100 };
	int m_maxHp{ 100 };
	int m_mp{ 50 };
	int m_maxMp{ 50 };
	int m_score{ 0 };
	int m_money{ 100 }; // 所持金
	enum class State { NORMAL, ATTACK};
	State m_state{ State::NORMAL };
	
	// レベル・経験値システム
	int m_level = 1;
	int m_exp = 0;
	int m_expToNextLevel = 100;
	static constexpr int HP_PER_LEVEL = 20;
	static constexpr int MP_PER_LEVEL = 10;
	static constexpr int ATK_PER_LEVEL = 5;
	int m_baseAttack = 10;  // 基礎攻撃力
	
	// インベントリシステム
	std::unique_ptr<Inventory> m_inventory;
	std::unique_ptr<ItemMenuUI> m_itemMenuUI;
	
	// 攻撃システム
	int m_comboCount = 0;          // コンボカウント
	Stopwatch m_comboTimer{};      // コンボタイマー
	Stopwatch m_specialCooldown{}; // 特殊攻撃のクールダウン
	
	// アイテム通知システム
	struct ItemPickupNotification {
		s3d::String itemName;
		s3d::ColorF itemColor;
		double showTime;
		double alpha = 1.0;
	};
	std::vector<ItemPickupNotification> m_pickupNotifications;
	
	// ステータス効果
	struct StatusEffect {
		ItemEffect::Type type;
		int value;
		double duration;
		double remainingTime;
	};
	std::vector<StatusEffect> m_statusEffects;
	
	// スキルシステム
	std::array<std::unique_ptr<Skill>, 4> m_skills;

public:
	Player(GameWorld* owner, const Vec2& position);
	
	void Damage(int damage)override { 
		m_hp -= damage; 
		if (m_hp < 0) m_hp = 0;
	}
	
	void Update()override;
	void Draw() const override;
	void UpdateUI() override;

	MyCircle GetCollision() const { return { GetPosition(), RADIUS }; }
	int GetScore() const { return m_score; }
	int GetMoney() const { return m_money; }
	int GetHP() const { return m_hp; }
	int GetMaxHP() const { return m_maxHp; }
	int GetMP() const { return m_mp; }
	int GetMaxMP() const { return m_maxMp; }
	
	// レベル・経験値関連
	void GainExp(int amount);
	void LevelUp();
	int GetLevel() const { return m_level; }
	int GetExp() const { return m_exp; }
	int GetExpToNextLevel() const { return m_expToNextLevel; }
	int GetAttack() const;  // ステータス効果込みの攻撃力
	
	// アイテム関連
	bool AddItem(int itemId, int quantity = 1);
	bool AddItem(const ItemData* itemData, int quantity = 1);
	void AddMoney(int amount) { m_money += amount; }
	void ShowItemPickupNotification(const ItemData* itemData);
	
	// ステータス効果
	void ApplyItemEffect(const ItemEffect& effect);
	void Heal(int amount);
	void RestoreMP(int amount);
	
	// スキル関連
	void InitializeSkills();
	void UseSkill(int index);  // 0=Q, 1=E, 2=R, 3=F
	void DrawSkillUI() const;

private:
	void attack();
	void performSpecialAttack();  // 特殊攻撃
	void UpdateStatusEffects();
	void UpdateCombos();
	void UpdatePickupNotifications();
	void DrawStatusBar() const;
	void DrawStatusEffects() const;
	void DrawAttackUI() const;    // 攻撃UI表示
	void DrawPickupNotifications() const; // アイテム取得通知表示
	void HandleInventoryInput();
	void HandleAttackInput();     // 攻撃入力処理
	void HandleSkillInput();      // スキル入力処理
};
