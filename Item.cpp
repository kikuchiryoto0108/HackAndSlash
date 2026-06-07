#include "stdafx.h"
#include "Item.h"
#include "ItemDatabase.h"

// アイテムタイプに応じた絵文字を取得するヘルパー関数
s3d::Icon GetIconEmoji(ItemType type)
{
	switch (type)
	{
	case ItemType::Consumable:  return 0x1F9EA_icon; // 試験管
	case ItemType::Weapon:      return 0x2694_icon;  // 剣
	case ItemType::Armor: return 0x1F6E1_icon; // 盾
	case ItemType::Material:    return 0x1F48E_icon; // 宝石
	case ItemType::Treasure:    return 0x1F4B0_icon; // 金袋
	case ItemType::Quest:     return 0x1F4DC_icon; // 巻物
	default:   return 0xF0CE5_icon; // デフォルト
	}
}

Item::Item(GameWorld* owner, const Vec2& position, int itemId) 
	: GameObject(owner, "Item")
{
	SetPosition(position);
	m_itemData = ItemDatabase::GetInstance().GetItemData(itemId);
	m_dropTime = s3d::Time::GetSec();
	m_bobOffset = s3d::Random(s3d::Math::TwoPi);
	
	// アイテムタイプに応じたテクスチャ登録
	if (m_itemData)
	{
		s3d::TextureAsset::Register(U"Item_" + m_itemData->iconName, 
			GetIconEmoji(m_itemData->type), 64);
	}
	else
	{
		// デフォルトアイテム
		s3d::TextureAsset::Register(U"Item_default", 0xF0CE5_icon, 64);
	}
}

Item::Item(GameWorld* owner, const Vec2& position, const ItemData* itemData)
	: GameObject(owner, "Item"), m_itemData(itemData)
{
	SetPosition(position);
	m_dropTime = s3d::Time::GetSec();
	m_bobOffset = s3d::Random(s3d::Math::TwoPi);
	
	if (m_itemData)
	{
		s3d::TextureAsset::Register(U"Item_" + m_itemData->iconName, 
			GetIconEmoji(m_itemData->type), 64);
	}
	else
	{
		s3d::TextureAsset::Register(U"Item_default", 0xF0CE5_icon, 64);
	}
}

void Item::Update()
{
	UpdateFloatingAnimation();
	
	// 一定時間後に消滅
	const double LIFETIME = 60.0; // 60秒
	if (s3d::Time::GetSec() - m_dropTime > LIFETIME)
	{
		Destroy();
	}
}

void Item::Draw() const
{
	// 浮遊アニメーション
	double bobY = s3d::Math::Sin((s3d::Time::GetSec() - m_dropTime) * 2.0 + m_bobOffset) * 3.0;
	Vec2 drawPos = GetPosition() + Vec2(0, bobY);
	
	// レア度エフェクト描画
	DrawRarityEffect();
	
	// アイテムの描画
	if (m_itemData)
	{
		s3d::TextureAsset(U"Item_" + m_itemData->iconName).drawAt(drawPos);
		
		// アイテム名表示（近くにいる時）
		// TODO: プレイヤーとの距離を確認して名前表示
	}
	else
	{
		s3d::TextureAsset(U"Item_default").drawAt(drawPos);
	}
	
	// 拾取可能範囲の表示（デバッグ用）
	if (false) // デバッグモードの時のみ
	{
		s3d::Circle(GetPosition(), RADIUS).drawFrame(2, s3d::ColorF(0.5, 0.5, 1.0, 0.3));
	}
}

void Item::DrawRarityEffect() const
{
	if (!m_itemData) return;
	
	double time = s3d::Time::GetSec() - m_dropTime;
	double alpha = 1.0;
	
	// ドロップ直後のフェードイン
	if (time < 0.5)
	{
		alpha = time / 0.5;
	}
	// 消滅前のフェードアウト
	else if (time > 55.0)
	{
		alpha = (60.0 - time) / 5.0;
	}
	
	s3d::ColorF effectColor = m_itemData->GetRarityColor();
	effectColor.a = alpha * 0.6;
	
	// レア度に応じたエフェクト
	switch (m_itemData->rarity)
	{
	case ItemRarity::Uncommon:
		// 緑の輝き
		s3d::Circle(GetPosition(), RADIUS * 1.2).drawFrame(3, effectColor);
		break;
		
	case ItemRarity::Rare:
		// 青のパルス
		{
			double pulseScale = 1.0 + s3d::Math::Sin(time * 4.0) * 0.2;
			s3d::Circle(GetPosition(), RADIUS * pulseScale).drawFrame(2, effectColor);
		}
		break;
		
	case ItemRarity::Epic:
		// 紫の回転エフェクト
		for (int i = 0; i < 6; ++i)
		{
			double angle = time * 2.0 + i * s3d::Math::TwoPi / 6;
			Vec2 offset(s3d::Math::Cos(angle) * (RADIUS + 10), s3d::Math::Sin(angle) * (RADIUS + 10));
			s3d::Circle(GetPosition() + offset, 5).draw(effectColor);
		}
		break;
		
	case ItemRarity::Legendary:
		// 金の豪華なエフェクト
		{
			// 回転する円
			for (int i = 0; i < 8; ++i)
			{
				double angle = time * 3.0 + i * s3d::Math::TwoPi / 8;
				Vec2 offset(s3d::Math::Cos(angle) * (RADIUS + 15), s3d::Math::Sin(angle) * (RADIUS + 15));
				s3d::Circle(GetPosition() + offset, 4).draw(effectColor);
			}
			
			// 中央のパルス
			double pulseScale = 1.0 + s3d::Math::Sin(time * 6.0) * 0.3;
			s3d::Circle(GetPosition(), RADIUS * pulseScale * 1.5).drawFrame(4, effectColor);
		}
		break;
	}
}

void Item::UpdateFloatingAnimation()
{
	// 浮遊アニメーションは描画で行うため、ここでは特に処理なし
	// 必要に応じて物理的な浮遊移動を追加可能
}

