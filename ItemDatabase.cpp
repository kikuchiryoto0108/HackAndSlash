#include "stdafx.h"
#include "ItemDatabase.h"

std::unique_ptr<ItemDatabase> ItemDatabase::s_instance = nullptr;

ItemDatabase& ItemDatabase::GetInstance()
{
	if (!s_instance)
	{
		s_instance = std::unique_ptr<ItemDatabase>(new ItemDatabase());
		s_instance->InitializeItems();
	}
	return *s_instance;
}

void ItemDatabase::InitializeItems()
{
	// 消耗品
	auto healPotion = std::make_unique<ItemData>(
		1, U"ヒーリングポーション", U"HPを50回復する",
		ItemType::Consumable, ItemRarity::Common, 10, 25, U"heal_potion", true);
	healPotion->effect.type = ItemEffect::Type::HealHP;
	healPotion->effect.value = 50;
	RegisterItem(std::move(healPotion));

	auto manaPotion = std::make_unique<ItemData>(
   2, U"マナポーション", U"MPを30回復する",
		ItemType::Consumable, ItemRarity::Common, 10, 20, U"mana_potion", true);
	manaPotion->effect.type = ItemEffect::Type::HealMP;
	manaPotion->effect.value = 30;
	RegisterItem(std::move(manaPotion));

	auto hiHealPotion = std::make_unique<ItemData>(
		3, U"上級ヒーリングポーション", U"HPを150回復する",
	ItemType::Consumable, ItemRarity::Rare, 5, 75, U"heal_potion_hi", true);
	hiHealPotion->effect.type = ItemEffect::Type::HealHP;
	hiHealPotion->effect.value = 150;
	RegisterItem(std::move(hiHealPotion));

	auto staminaDrink = std::make_unique<ItemData>(
		4, U"スタミナドリンク", U"移動速度を10秒間上昇させる",
		ItemType::Consumable, ItemRarity::Uncommon, 5, 40, U"stamina_drink", true);
	staminaDrink->effect.type = ItemEffect::Type::SpeedBoost;
	staminaDrink->effect.value = 50;
	staminaDrink->effect.duration = 10.0;
	RegisterItem(std::move(staminaDrink));

	// 新しい消耗品を追加
	auto megaHealPotion = std::make_unique<ItemData>(
		5, U"メガヒーリングポーション", U"HPを全回復する",
 ItemType::Consumable, ItemRarity::Epic, 3, 200, U"mega_heal", true);
	megaHealPotion->effect.type = ItemEffect::Type::HealHP;
	megaHealPotion->effect.value = 999; // 実質全回復
	RegisterItem(std::move(megaHealPotion));

	auto attackBoostPotion = std::make_unique<ItemData>(
		6, U"力の薬", U"攻撃力を30秒間+30上昇",
		ItemType::Consumable, ItemRarity::Rare, 3, 120, U"str_potion", true);
	attackBoostPotion->effect.type = ItemEffect::Type::AttackBoost;
	attackBoostPotion->effect.value = 30;
	attackBoostPotion->effect.duration = 30.0;
	RegisterItem(std::move(attackBoostPotion));

	auto defenseBoostPotion = std::make_unique<ItemData>(
		7, U"守りの薬", U"防御力を30秒間+20上昇",
		ItemType::Consumable, ItemRarity::Rare, 3, 100, U"def_potion", true);
	defenseBoostPotion->effect.type = ItemEffect::Type::DefenseBoost;
	defenseBoostPotion->effect.value = 20;
	defenseBoostPotion->effect.duration = 30.0;
	RegisterItem(std::move(defenseBoostPotion));

	// 武器
	RegisterItem(std::make_unique<ItemData>(
		101, U"鉄の剣", U"基本的な鉄製の剣。攻撃力+10",
		ItemType::Weapon, ItemRarity::Common, 1, 100, U"iron_sword", false));

	RegisterItem(std::make_unique<ItemData>(
		102, U"炎の剣", U"炎の力が宿った剣。攻撃力+25",
		ItemType::Weapon, ItemRarity::Rare, 1, 500, U"fire_sword", false));

	RegisterItem(std::make_unique<ItemData>(
		103, U"ドラゴンキラー", U"伝説のドラゴンスレイヤー。攻撃力+50",
		ItemType::Weapon, ItemRarity::Legendary, 1, 2000, U"dragon_slayer", false));

	// 新しい武器を追加
	RegisterItem(std::make_unique<ItemData>(
		104, U"雷鳴の槍", U"雷の力を宿した槍。攻撃力+35",
		ItemType::Weapon, ItemRarity::Epic, 1, 800, U"thunder_spear", false));

	// 防具
	RegisterItem(std::make_unique<ItemData>(
		201, U"革の鎧", U"基本的な革製の鎧。防御力+5",
		ItemType::Armor, ItemRarity::Common, 1, 80, U"leather_armor", false));

	RegisterItem(std::make_unique<ItemData>(
		202, U"プレートメイル", U"重厚な鋼鉄の鎧。防御力+20",
		ItemType::Armor, ItemRarity::Rare, 1, 400, U"plate_mail", false));

	RegisterItem(std::make_unique<ItemData>(
		203, U"ミスリルアーマー", U"軽量で強固な魔法の鎧。防御力+35",
		ItemType::Armor, ItemRarity::Epic, 1, 1500, U"mithril_armor", false));

	// 新しい防具を追加
	RegisterItem(std::make_unique<ItemData>(
		204, U"竜鱗の盾", U"ドラゴンの鱗で作られた盾。防御力+15",
		ItemType::Armor, ItemRarity::Rare, 1, 600, U"dragon_shield", false));

	// 素材
	RegisterItem(std::make_unique<ItemData>(
		301, U"鉄鉱石", U"武器や防具の素材となる鉱石",
		ItemType::Material, ItemRarity::Common, 20, 5, U"iron_ore", false));

	RegisterItem(std::make_unique<ItemData>(
		302, U"ドラゴンの鱗", U"ドラゴンから採取した貴重な鱗",
		ItemType::Material, ItemRarity::Epic, 5, 200, U"dragon_scale", false));

	RegisterItem(std::make_unique<ItemData>(
		303, U"魔法の結晶", U"魔力が込められた美しい結晶",
		ItemType::Material, ItemRarity::Rare, 10, 100, U"magic_crystal", false));

	// 新しい素材を追加
	RegisterItem(std::make_unique<ItemData>(
		304, U"星の欠片", U"隕石から採取した神秘的な欠片",
		ItemType::Material, ItemRarity::Legendary, 1, 1000, U"star_fragment", false));

	RegisterItem(std::make_unique<ItemData>(
		305, U"薬草", U"ポーション作成に使用する薬草",
		ItemType::Material, ItemRarity::Common, 50, 3, U"herb", false));

	// 宝物
	RegisterItem(std::make_unique<ItemData>(
		401, U"金貨", U"価値の高い金貨",
		ItemType::Treasure, ItemRarity::Common, 100, 1, U"gold_coin", false));

	RegisterItem(std::make_unique<ItemData>(
		402, U"ダイヤモンド", U"輝く美しいダイヤモンド",
		ItemType::Treasure, ItemRarity::Legendary, 1, 1000, U"diamond", false));

	// 新しい宝物を追加
	RegisterItem(std::make_unique<ItemData>(
		403, U"ルビー", U"真紅に輝く宝石",
		ItemType::Treasure, ItemRarity::Epic, 1, 500, U"ruby", false));

	RegisterItem(std::make_unique<ItemData>(
		404, U"サファイア", U"深い青色の美しい宝石",
		ItemType::Treasure, ItemRarity::Rare, 1, 300, U"sapphire", false));

	// クエストアイテム
	RegisterItem(std::make_unique<ItemData>(
		501, U"古い手紙", U"重要な情報が記されている",
		ItemType::Quest, ItemRarity::Common, 1, 0, U"old_letter", false));

	RegisterItem(std::make_unique<ItemData>(
		502, U"王の印璽", U"王家の証となる特別な指輪",
		ItemType::Quest, ItemRarity::Legendary, 1, 0, U"royal_ring", false));

	// 新しいクエストアイテムを追加
	RegisterItem(std::make_unique<ItemData>(
		503, U"魔法の鍵", U"封印された扉を開く神秘の鍵",
		ItemType::Quest, ItemRarity::Epic, 1, 0, U"magic_key", false));
}

const ItemData* ItemDatabase::GetItemData(int itemId) const
{
	auto it = m_items.find(itemId);
	return (it != m_items.end()) ? it->second.get() : nullptr;
}

void ItemDatabase::RegisterItem(std::unique_ptr<ItemData> itemData)
{
	if (itemData)
	{
		m_items[itemData->id] = std::move(itemData);
	}
}

int ItemDatabase::GetRandomItemId() const
{
	if (m_items.empty()) return -1;

	std::vector<int> ids;
	for (const auto& pair : m_items)
	{
		ids.push_back(pair.first);
	}

	return ids[s3d::Random(ids.size() - 1)];
}

std::vector<const ItemData*> ItemDatabase::FindItems(ItemType type) const
{
	std::vector<const ItemData*> result;
	for (const auto& pair : m_items)
	{
		if (pair.second->type == type)
		{
			result.push_back(pair.second.get());
		}
	}
	return result;
}

std::vector<const ItemData*> ItemDatabase::FindItems(ItemRarity rarity) const
{
	std::vector<const ItemData*> result;
	for (const auto& pair : m_items)
	{
		if (pair.second->rarity == rarity)
		{
			result.push_back(pair.second.get());
		}
	}
	return result;
}
