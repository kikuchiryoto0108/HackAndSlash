#pragma once

#include "GameObject.h"
#include "ItemDatabase.h"

/// @brief ランダムアイテムドロップ機能を提供するクラス
class ItemDropper
{
public:
 /// @brief 指定位置にランダムアイテムをドロップ
    /// @param gameWorld ゲーム世界
    /// @param position ドロップ位置
    /// @param rarityBonus レア度ボーナス（0.0-1.0）
    static void DropRandomItem(GameWorld* gameWorld, const Vec2& position, double rarityBonus = 0.0);
    
    /// @brief 指定アイテムをドロップ
    /// @param gameWorld ゲーム世界
    /// @param position ドロップ位置
    /// @param itemId アイテムID
    /// @param quantity 数量
    static void DropItem(GameWorld* gameWorld, const Vec2& position, int itemId, int quantity = 1);
    
    /// @brief 敵撃破時のアイテムドロップ
    /// @param gameWorld ゲーム世界
    /// @param position ドロップ位置
    /// @param enemyLevel 敵のレベル
    static void DropItemsFromEnemy(GameWorld* gameWorld, const Vec2& position, int enemyLevel = 1);
    
    /// @brief 宝箱からのアイテムドロップ
    /// @param gameWorld ゲーム世界
    /// @param position ドロップ位置
    /// @param isRareChest レアな宝箱かどうか
  static void DropItemsFromChest(GameWorld* gameWorld, const Vec2& position, bool isRareChest = false);

private:
  /// @brief レア度に基づいてランダムアイテムを選択
    static const ItemData* GetRandomItemByRarity(ItemRarity targetRarity);
  
    /// @brief レア度の抽選
    static ItemRarity RollRarity(double rarityBonus = 0.0);
    
    /// @brief ドロップ位置を散らす
    static Vec2 GetScatteredPosition(const Vec2& center, double radius = 50.0);
};
