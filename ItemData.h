#pragma once

#include "ItemType.h"

/// @brief アイテムデータを管理するクラス
class ItemData
{
public:
	int id;     // アイテムID
	s3d::String name;    // アイテム名
	s3d::String description;        // 説明
	ItemType type;            // アイテム種類
	ItemRarity rarity;              // レア度
	ItemEffect effect;      // 効果
	int maxStack;  // 最大スタック数
	int sellPrice;                  // 売却価格
	s3d::String iconName;     // アイコン名
	bool usable;  // 使用可能か

public:
	ItemData(int _id, const s3d::String& _name, const s3d::String& _description,
		ItemType _type, ItemRarity _rarity, int _maxStack = 1, int _sellPrice = 10,
		   const s3d::String& _iconName = U"", bool _usable = false)
		: id(_id), name(_name), description(_description), type(_type), rarity(_rarity)
		, maxStack(_maxStack), sellPrice(_sellPrice), iconName(_iconName), usable(_usable)
	{
	}

	/// @brief レア度に応じた色を取得
	s3d::ColorF GetRarityColor() const
	{
		switch (rarity)
		{
		case ItemRarity::Common:    return s3d::ColorF(0.8, 0.8, 0.8);     // グレー
		case ItemRarity::Uncommon:  return s3d::ColorF(0.2, 0.8, 0.2); // 緑
		case ItemRarity::Rare: return s3d::ColorF(0.2, 0.5, 1.0);     // 青
		case ItemRarity::Epic:      return s3d::ColorF(0.7, 0.2, 0.9);     // 紫
		case ItemRarity::Legendary: return s3d::ColorF(1.0, 0.8, 0.1);  // 金
		default:          return s3d::ColorF(1.0, 1.0, 1.0);     // 白
		}
	}

	/// @brief アイテム種類に応じた色を取得
	s3d::ColorF GetTypeColor() const
	{
		switch (type)
		{
		case ItemType::Consumable:  return s3d::ColorF(0.9, 0.4, 0.4);     // 赤
		case ItemType::Weapon:      return s3d::ColorF(0.8, 0.6, 0.3);     // オレンジ
		case ItemType::Armor:  return s3d::ColorF(0.5, 0.5, 0.8);     // 青紫
		case ItemType::Material:    return s3d::ColorF(0.6, 0.4, 0.2);     // 茶
		case ItemType::Treasure:    return s3d::ColorF(1.0, 0.8, 0.1);     // 金
		case ItemType::Quest:       return s3d::ColorF(0.3, 0.8, 0.9);     // シアン
		default:        return s3d::ColorF(0.7, 0.7, 0.7);     // グレー
		}
	}
};
