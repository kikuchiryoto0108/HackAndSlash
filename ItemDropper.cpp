#include "stdafx.h"
#include "ItemDropper.h"
#include "GameWorld.h"
#include "Item.h"

void ItemDropper::DropRandomItem(GameWorld* gameWorld, const Vec2& position, double rarityBonus)
{
	if (!gameWorld) return;

	ItemRarity rarity = RollRarity(rarityBonus);
	const ItemData* itemData = GetRandomItemByRarity(rarity);

	if (itemData)
	{
		Vec2 dropPos = GetScatteredPosition(position);
		gameWorld->Registar(new Item(gameWorld, dropPos, itemData));
	}
}

void ItemDropper::DropItem(GameWorld* gameWorld, const Vec2& position, int itemId, int quantity)
{
	if (!gameWorld) return;

	const ItemData* itemData = ItemDatabase::GetInstance().GetItemData(itemId);
	if (!itemData) return;

	// 数量に応じて複数ドロップ
	for (int i = 0; i < quantity; ++i)
	{
		Vec2 dropPos = GetScatteredPosition(position);
		gameWorld->Registar(new Item(gameWorld, dropPos, itemData));
	}
}

void ItemDropper::DropItemsFromEnemy(GameWorld* gameWorld, const Vec2& position, int enemyLevel)
{
	if (!gameWorld) return;

	// レベルに応じたレア度ボーナス
	double rarityBonus = enemyLevel * 0.1;

	// ドロップ判定（70%の確率でドロップ）
	if (s3d::Random(0.0, 1.0) < 0.7)
	{
		DropRandomItem(gameWorld, position, rarityBonus);
	}

	// 稀に複数ドロップ（15%の確率）
	if (s3d::Random(0.0, 1.0) < 0.15)
	{
		DropRandomItem(gameWorld, position, rarityBonus);
	}

	// 金貨は常にドロップ
	int goldAmount = s3d::Random(1, 5) * enemyLevel;
	for (int i = 0; i < goldAmount; ++i)
	{
		DropItem(gameWorld, position, 401); // 金貨のアイテムID
	}
}

void ItemDropper::DropItemsFromChest(GameWorld* gameWorld, const Vec2& position, bool isRareChest)
{
	if (!gameWorld) return;

	double rarityBonus = isRareChest ? 0.8 : 0.3;

	// 宝箱からは必ず複数アイテムドロップ
	int itemCount = s3d::Random(3, isRareChest ? 8 : 5);

	for (int i = 0; i < itemCount; ++i)
	{
		DropRandomItem(gameWorld, position, rarityBonus);
	}

	// レアチェストの場合は特別なアイテム確定ドロップ
	if (isRareChest)
	{
		// 高レアアイテムを確定ドロップ
		ItemRarity guaranteedRarity = (s3d::Random(0.0, 1.0) < 0.3) ? ItemRarity::Legendary : ItemRarity::Epic;
		const ItemData* rareItem = GetRandomItemByRarity(guaranteedRarity);
		if (rareItem)
		{
			Vec2 dropPos = GetScatteredPosition(position);
			gameWorld->Registar(new Item(gameWorld, dropPos, rareItem));
		}
	}
}

const ItemData* ItemDropper::GetRandomItemByRarity(ItemRarity targetRarity)
{
	auto& database = ItemDatabase::GetInstance();
	auto itemsOfRarity = database.FindItems(targetRarity);

	if (itemsOfRarity.empty())
	{
		// 該当レア度のアイテムがない場合は全アイテムからランダム選択
		int randomId = database.GetRandomItemId();
		return database.GetItemData(randomId);
	}

	// 指定レア度のアイテムからランダム選択
	int randomIndex = s3d::Random(static_cast<int>(itemsOfRarity.size() - 1));
	return itemsOfRarity[randomIndex];
}

ItemRarity ItemDropper::RollRarity(double rarityBonus)
{
	double roll = s3d::Random(0.0, 1.0) + rarityBonus;

	// レア度確率（ボーナス込み）
	if (roll >= 0.98)  return ItemRarity::Legendary;  // 2% + ボーナス
	if (roll >= 0.92)  return ItemRarity::Epic; // 6% + ボーナス
	if (roll >= 0.75)  return ItemRarity::Rare;   // 17% + ボーナス
	if (roll >= 0.50)  return ItemRarity::Uncommon;   // 25% + ボーナス

	return ItemRarity::Common;  // 50% - ボーナス影響
}

Vec2 ItemDropper::GetScatteredPosition(const Vec2& center, double radius)
{
	double angle = s3d::Random(0.0, s3d::Math::TwoPi);
	double distance = s3d::Random(0.0, radius);
	return center + Vec2(s3d::Math::Cos(angle) * distance, s3d::Math::Sin(angle) * distance);
}
