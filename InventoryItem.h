#pragma once

#include "ItemData.h"

/// @brief インベントリ内の個別アイテムを表すクラス
class InventoryItem
{
public:
    const ItemData* itemData;       // アイテムデータへの参照
    int quantity;        // 所持数量
    bool isEquipped;   // 装備中かどうか
    double acquiredTime;      // 取得時刻

public:
    InventoryItem(const ItemData* _itemData, int _quantity = 1)
        : itemData(_itemData), quantity(_quantity), isEquipped(false), acquiredTime(s3d::Time::GetSec())
 {}

    /// @brief アイテムを追加（スタック可能な場合）
    /// @param count 追加する数量
    /// @return 実際に追加できた数量
    int AddQuantity(int count)
    {
        int canAdd = s3d::Min(count, itemData->maxStack - quantity);
        quantity += canAdd;
        return canAdd;
    }

    /// @brief アイテムを消費
    /// @param count 消費する数量
    /// @return 実際に消費した数量
    int ConsumeQuantity(int count)
 {
        int canConsume = s3d::Min(count, quantity);
    quantity -= canConsume;
 return canConsume;
    }

    /// @brief スタック可能か判定
    bool CanStack(const ItemData* other) const
    {
        return itemData == other && quantity < itemData->maxStack;
    }

  /// @brief 空きスロット数を取得
    int GetAvailableStack() const
    {
        return itemData->maxStack - quantity;
    }
};
