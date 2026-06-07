#pragma once

#include "GameObject.h"
#include "ItemData.h"

class Item : public GameObject
{
private:
	static constexpr double RADIUS = 32.0;
	const ItemData* m_itemData;  // アイテムデータへの参照
	double m_dropTime;  // ドロップされた時刻
	double m_bobOffset; // 浮遊アニメーション用オフセット
	
public:
	Item(GameWorld* owner, const Vec2& position, int itemId);
	Item(GameWorld* owner, const Vec2& position, const ItemData* itemData);

	void Update() override;
	void Draw() const override;

	MyCircle GetCollision() const { return { GetPosition(), RADIUS }; }
	
	/// @brief アイテムデータを取得
	const ItemData* GetItemData() const { return m_itemData; }
	
	/// @brief アイテムIDを取得
	int GetItemId() const { return m_itemData ? m_itemData->id : -1; }

private:
	/// @brief レア度に応じたエフェクトを描画
	void DrawRarityEffect() const;
	
	/// @brief 浮遊アニメーション
	void UpdateFloatingAnimation();
};

