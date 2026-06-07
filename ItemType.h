#pragma once

/// @brief アイテムの種類を表す列挙型
enum class ItemType
{
	Consumable,     // 消耗品（ポーション、食べ物など）
	Weapon,         // 武器
	Armor,  // 防具
	Material,       // 素材
	Treasure, // 宝物
	Quest           // クエストアイテム
};

/// @brief アイテムのレア度を表す列挙型
enum class ItemRarity
{
	Common,         // 一般（白）
	Uncommon,       // 非一般（緑）
	Rare,   // レア（青）
	Epic,           // エピック（紫）
	Legendary       // 伝説（金）
};

/// @brief アイテムの効果を表す構造体
struct ItemEffect
{
	enum class Type
	{
		None,
		HealHP,         // HP回復
		HealMP,  // MP回復
		AttackBoost,    // 攻撃力上昇
		DefenseBoost,   // 防御力上昇
		SpeedBoost      // 移動速度上昇
	} type = Type::None;

	int value = 0;      // 効果の値
	double duration = 0.0;  // 持続時間（秒、0なら瞬間効果）
};
