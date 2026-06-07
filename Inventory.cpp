#include "stdafx.h"
#include "Inventory.h"
#include <algorithm>

int Inventory::AddItem(const ItemData* itemData, int quantity)
{
	if (!itemData || quantity <= 0) return 0;

	int remainingQuantity = quantity;

	// スタック可能なアイテムを探して追加
	if (itemData->maxStack > 1)
	{
		for (auto& item : m_items)
		{
			if (item->CanStack(itemData))
			{
				int added = item->AddQuantity(remainingQuantity);
				remainingQuantity -= added;
				if (remainingQuantity == 0) break;
			}
		}
	}

	// 新しいスロットを作成
	while (remainingQuantity > 0 && m_items.size() < m_maxSlots)
	{
		int stackSize = s3d::Min(remainingQuantity, itemData->maxStack);
		m_items.push_back(std::make_unique<InventoryItem>(itemData, stackSize));
		remainingQuantity -= stackSize;
	}

	return quantity - remainingQuantity;
}

int Inventory::RemoveItem(int itemId, int quantity)
{
	int removedQuantity = 0;

	for (auto it = m_items.begin(); it != m_items.end();)
	{
		if ((*it)->itemData->id == itemId)
		{
			int toRemove = s3d::Min(quantity - removedQuantity, (*it)->quantity);
			removedQuantity += (*it)->ConsumeQuantity(toRemove);

			if ((*it)->quantity == 0)
			{
				it = m_items.erase(it);
			}
			else
			{
				++it;
			}

			if (removedQuantity >= quantity) break;
		}
		else
		{
			++it;
		}
	}

	return removedQuantity;
}

bool Inventory::UseItem(size_t slotIndex)
{
	if (slotIndex >= m_items.size()) return false;

	InventoryItem* item = m_items[slotIndex].get();
	if (!item || !item->itemData->usable) return false;

	// ★追加: アイテム効果を適用
	if (m_onItemUsed && item->itemData->effect.type != ItemEffect::Type::None)
	{
		m_onItemUsed(item->itemData->effect);
	}

	// 消耗品の場合は数量を減らす
	if (item->itemData->type == ItemType::Consumable)
	{
		item->ConsumeQuantity(1);
		if (item->quantity == 0)
		{
			m_items.erase(m_items.begin() + slotIndex);
		}
	}

	return true;
}

void Inventory::ToggleEquip(size_t slotIndex)
{
	if (slotIndex >= m_items.size()) return;

	InventoryItem* item = m_items[slotIndex].get();
	if (!item) return;

	// 装備可能なアイテムタイプかチェック
	if (item->itemData->type == ItemType::Weapon || item->itemData->type == ItemType::Armor)
	{
		// 同じタイプの他のアイテムの装備を解除
		if (!item->isEquipped)
		{
			for (auto& otherItem : m_items)
			{
				if (otherItem->itemData->type == item->itemData->type)
				{
					otherItem->isEquipped = false;
				}
			}
		}

		item->isEquipped = !item->isEquipped;
	}
}

void Inventory::SortItems(SortType sortType)
{
	if (sortType == SortType::None) return;

	// 同じソートタイプの場合は昇順/降順を切り替え
	if (m_currentSort == sortType)
	{
		m_sortAscending = !m_sortAscending;
	}
	else
	{
		m_currentSort = sortType;
		m_sortAscending = true;
	}

	std::sort(m_items.begin(), m_items.end(), [this](const std::unique_ptr<InventoryItem>& a, const std::unique_ptr<InventoryItem>& b)
	{
		return CompareItems(a, b);
	});
}

std::vector<InventoryItem*> Inventory::GetFilteredItems() const
{
	std::vector<InventoryItem*> filtered;

	for (const auto& item : m_items)
	{
		if (PassesFilter(item.get()))
		{
			filtered.push_back(item.get());
		}
	}

	return filtered;
}

InventoryItem* Inventory::GetItem(size_t slotIndex) const
{
	if (slotIndex >= m_items.size()) return nullptr;
	return m_items[slotIndex].get();
}

InventoryItem* Inventory::FindItem(int itemId) const
{
	for (const auto& item : m_items)
	{
		if (item->itemData->id == itemId)
		{
			return item.get();
		}
	}
	return nullptr;
}

InventoryItem* Inventory::FindStackableItem(const ItemData* itemData)
{
	for (auto& item : m_items)
	{
		if (item->CanStack(itemData))
		{
			return item.get();
		}
	}
	return nullptr;
}

bool Inventory::PassesFilter(const InventoryItem* item) const
{
	if (!item) return false;

	if (m_filter.useTypeFilter && item->itemData->type != m_filter.typeFilter)
		return false;

	if (m_filter.useRarityFilter && item->itemData->rarity != m_filter.rarityFilter)
		return false;

	if (m_filter.showEquippedOnly && !item->isEquipped)
		return false;

	if (m_filter.showUsableOnly && !item->itemData->usable)
		return false;

	return true;
}

bool Inventory::CompareItems(const std::unique_ptr<InventoryItem>& a, const std::unique_ptr<InventoryItem>& b) const
{
	// nullptrチェック
	if (!a || !a->itemData) return false;
	if (!b || !b->itemData) return true;

	// 降順の場合は比較対象を入れ替え
	const auto& left = m_sortAscending ? a : b;
	const auto& right = m_sortAscending ? b : a;

	switch (m_currentSort)
	{
	case SortType::Name:
		return left->itemData->name < right->itemData->name;

	case SortType::Type:
		return static_cast<int>(left->itemData->type) < static_cast<int>(right->itemData->type);

	case SortType::Rarity:
		return static_cast<int>(left->itemData->rarity) < static_cast<int>(right->itemData->rarity);

	case SortType::Quantity:
		return left->quantity < right->quantity;

	case SortType::AcquiredTime:
		return left->acquiredTime < right->acquiredTime;

	default:
		return false;
	}
}
