#pragma once

#include "ItemData.h"
#include "InventoryItem.h"
#include <vector>
#include <memory>

/// @brief ソート方式を表す列挙型
enum class SortType
{
    None,    // ソートなし
    Name,    // 名前順
    Type,  // 種類順
    Rarity,       // レア度順
    Quantity,       // 数量順
    AcquiredTime    // 取得時刻順
};

/// @brief フィルター設定を表す構造体
struct FilterSettings
{
    ItemType typeFilter = ItemType::Consumable;    // 種類フィルター
  bool useTypeFilter = false;    // 種類フィルターを使用するか
    ItemRarity rarityFilter = ItemRarity::Common;  // レア度フィルター
    bool useRarityFilter = false;        // レア度フィルターを使用するか
    bool showEquippedOnly = false;       // 装備品のみ表示
    bool showUsableOnly = false;  // 使用可能アイテムのみ表示
};

/// @brief インベントリシステムを管理するクラス
class Inventory
{
private:
    std::vector<std::unique_ptr<InventoryItem>> m_items;  // アイテム一覧
    size_t m_maxSlots = 50;          // 最大スロット数
    SortType m_currentSort = SortType::None;      // 現在のソート方式
    bool m_sortAscending = true;           // 昇順ソートかどうか
    FilterSettings m_filter;          // フィルター設定

	std::function<void(const ItemEffect&)> m_onItemUsed;

public:
    Inventory(size_t maxSlots = 50) : m_maxSlots(maxSlots) {}

	// ★追加: コールバック設定メソッド
	void SetItemUsedCallback(std::function<void(const ItemEffect&)> callback)
	{
		m_onItemUsed = callback;
	}

    /// @brief アイテムを追加
    /// @param itemData 追加するアイテムのデータ
    /// @param quantity 数量
    /// @return 実際に追加できた数量
    int AddItem(const ItemData* itemData, int quantity = 1);

    /// @brief アイテムを削除
    /// @param itemId アイテムID
    /// @param quantity 削除数量
    /// @return 実際に削除した数量
    int RemoveItem(int itemId, int quantity = 1);

    /// @brief アイテムを使用
    /// @param slotIndex スロットインデックス
  /// @return 使用に成功したかどうか
    bool UseItem(size_t slotIndex);

    /// @brief アイテムの装備状態を切り替え
    /// @param slotIndex スロットインデックス
    void ToggleEquip(size_t slotIndex);

    /// @brief アイテムを並び替え
 void SortItems(SortType sortType);

    /// @brief フィルター適用されたアイテム一覧を取得
 std::vector<InventoryItem*> GetFilteredItems() const;

    /// @brief フィルター設定を更新
    void SetFilter(const FilterSettings& filter) { m_filter = filter; }
    const FilterSettings& GetFilter() const { return m_filter; }

    /// @brief ソート状態を取得
    SortType GetCurrentSort() const { return m_currentSort; }
    bool IsSortAscending() const { return m_sortAscending; }

    /// @brief インベントリの状態取得
    size_t GetItemCount() const { return m_items.size(); }
 size_t GetMaxSlots() const { return m_maxSlots; }
    size_t GetAvailableSlots() const { return m_maxSlots - m_items.size(); }

  /// @brief 特定スロットのアイテム取得
    InventoryItem* GetItem(size_t slotIndex) const;

    /// @brief アイテムIDで検索
    InventoryItem* FindItem(int itemId) const;

    /// @brief 全アイテムを取得
    const std::vector<std::unique_ptr<InventoryItem>>& GetAllItems() const { return m_items; }

    /// @brief インベントリをクリア
    void Clear() { m_items.clear(); }

private:
    /// @brief スタック可能なアイテムを検索
    InventoryItem* FindStackableItem(const ItemData* itemData);

    /// @brief フィルター条件に合うかチェック
    bool PassesFilter(const InventoryItem* item) const;

    /// @brief ソート比較関数
    bool CompareItems(const std::unique_ptr<InventoryItem>& a, const std::unique_ptr<InventoryItem>& b) const;
};
