#include "stdafx.h"
#include "TreasureChest.h"
#include "GameWorld.h"
#include "GameObjectContainer.h"
#include "ItemDropper.h"

TreasureChest::TreasureChest(GameWorld* owner, const Vec2& position, bool isRare)
	: GameObject(owner, "TreasureChest"), m_isRareChest(isRare)
{
	SetPosition(position);

	// アイコン登録
	if (m_isRareChest)
	{
		s3d::TextureAsset::Register(U"RareChest", 0x1F4B0_icon, 80); // 金袋
	}
	else
	{
		s3d::TextureAsset::Register(U"NormalChest", 0x1F381_icon, 80); // プレゼント箱
	}
}

void TreasureChest::Update()
{
	if (m_isOpened) return;

	// プレイヤーが近くにいてFキーが押された時に開く（Eキーから変更）
	if (IsPlayerNearby() && s3d::KeyF.down())
	{
		OpenChest();
	}
}

void TreasureChest::Draw() const
{
	s3d::ColorF chestColor = m_isOpened ? s3d::ColorF(0.5, 0.5, 0.5) : s3d::ColorF(1.0);

	if (m_isRareChest)
	{
		s3d::TextureAsset(U"RareChest").drawAt(GetPosition(), chestColor);

		// レアチェストの場合は特別なエフェクト
		if (!m_isOpened)
		{
			double time = s3d::Time::GetSec();
			s3d::ColorF glowColor = s3d::ColorF(1.0, 0.8, 0.0, 0.5 + 0.3 * s3d::Math::Sin(time * 4.0));
			s3d::Circle(GetPosition(), RADIUS * 1.3).drawFrame(3, glowColor);
		}
	}
	else
	{
		s3d::TextureAsset(U"NormalChest").drawAt(GetPosition(), chestColor);
	}

	// プレイヤーが近くにいる場合はインタラクションプロンプトを表示
	if (!m_isOpened && IsPlayerNearby())
	{
		DrawInteractionPrompt();
	}
}

bool TreasureChest::IsPlayerNearby() const
{
	GameObjectContainer players = GetOwner()->GetGameObjects("Player");
	if (players.GetCount() == 0) return false;

	Vec2 playerPos = players.GetGameObject(0)->GetPosition();
	double distance = (playerPos - GetPosition()).length();

	return distance <= m_interactionRange;
}

void TreasureChest::OpenChest()
{
	if (m_isOpened) return;

	m_isOpened = true;
	m_openAnimation.restart();

	// アイテムドロップ
	ItemDropper::DropItemsFromChest(GetOwner(), GetPosition(), m_isRareChest);
}

void TreasureChest::DrawInteractionPrompt() const
{
	Vec2 promptPos = GetPosition() + Vec2(0, -60);

	// 背景
	s3d::Rect promptBG(promptPos.x - 30, promptPos.y - 10, 60, 20);
	promptBG.draw(s3d::ColorF(0.0, 0.0, 0.0, 0.7));
	promptBG.drawFrame(1, s3d::ColorF(1.0, 1.0, 1.0));

	// テキスト
	s3d::FontAsset::Register(U"InteractionFont", 12);
	s3d::String promptText = m_isRareChest ? U"[F] レア宝箱" : U"[F] 宝箱";
	s3d::FontAsset(U"InteractionFont")(promptText)
		.drawAt(promptPos, s3d::ColorF(1.0, 1.0, 1.0));
}
