#pragma once

#include "ItemData.h"
#include <unordered_map>
#include <memory>

/// @brief アイテムデータベースを管理するクラス
class ItemDatabase
{
private:
 static std::unique_ptr<ItemDatabase> s_instance;
    std::unordered_map<int, std::unique_ptr<ItemData>> m_items;

private:
    ItemDatabase() = default;
    void InitializeItems();

public:
 static ItemDatabase& GetInstance();
    
    /// @brief アイテムデータを取得
    const ItemData* GetItemData(int itemId) const;
 
    /// @brief 全アイテムデータを取得
    const std::unordered_map<int, std::unique_ptr<ItemData>>& GetAllItems() const { return m_items; }
    
  /// @brief アイテムデータを登録
  void RegisterItem(std::unique_ptr<ItemData> itemData);

    /// @brief ランダムなアイテムIDを取得
 int GetRandomItemId() const;

    /// @brief 特定条件のアイテムを検索
    std::vector<const ItemData*> FindItems(ItemType type) const;
    std::vector<const ItemData*> FindItems(ItemRarity rarity) const;
};
