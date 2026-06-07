#pragma once

#include "Inventory.h"
#include "ItemDatabase.h"

/// @brief アイテムメニューの描画設定
struct ItemMenuStyle
{
	s3d::ColorF backgroundColor = s3d::ColorF(0.1, 0.1, 0.2, 0.95);
	s3d::ColorF borderColor = s3d::ColorF(0.4, 0.5, 0.7);
	s3d::ColorF slotColor = s3d::ColorF(0.15, 0.15, 0.2);
	s3d::ColorF slotHoverColor = s3d::ColorF(0.25, 0.25, 0.3);
	s3d::ColorF slotSelectedColor = s3d::ColorF(0.3, 0.4, 0.6);
	s3d::ColorF textColor = s3d::ColorF(0.9, 0.9, 0.9);
	s3d::ColorF buttonColor = s3d::ColorF(0.3, 0.3, 0.5);
	s3d::ColorF buttonHoverColor = s3d::ColorF(0.4, 0.4, 0.6);

	double slotSize = 70.0;
	double slotSpacing = 8.0;
	int slotsPerRow = 10;
	double buttonHeight = 35.0;
	double margin = 20.0;
	double headerHeight = 80.0;
	double infoPanelWidth = 400.0;
	double buttonAreaHeight = 60.0;
};

/// @brief アイテムメニューのUI管理クラス
class ItemMenuUI
{
private:
	Inventory* m_inventory;
	ItemMenuStyle m_style;
	s3d::Rect m_menuRect;
	s3d::Point m_menuPos;
	bool m_isOpen = false;

	// 操作状態
	int m_selectedSlot = -1;
	int m_hoveredSlot = -1;
	SortType m_lastSortType = SortType::None;
	FilterSettings m_currentFilter;

	// ドラッグ&ドロップ
	bool m_isDragging = false;
	int m_draggedSlot = -1;
	s3d::Point m_dragOffset;

	// アニメーション
	s3d::Stopwatch m_openAnimation;
	double m_targetAlpha = 0.0;
	double m_currentAlpha = 0.0;

	// エラーメッセージ
	s3d::String m_errorMessage;
	double m_errorMessageTime = 0.0;

	// UI要素の位置
	s3d::Rect m_headerRect;
	s3d::Rect m_slotsRect;
	s3d::Rect m_buttonRect;
	s3d::Rect m_infoRect;

	// ボタン
	std::vector<s3d::Rect> m_sortButtons;
	std::vector<s3d::Rect> m_filterButtons;
	s3d::Rect m_sellButton;
	s3d::Rect m_closeButton;
	s3d::Rect m_sellAllButton;
	s3d::Rect m_sortOrderButton;

	// プレイヤー参照（所持金表示用）
	class Player* m_player;

	std::function<void(bool)> m_onMenuStateChanged;  // コールバック

public:
	ItemMenuUI(Inventory* inventory, Player* player = nullptr);

	void SetMenuStateCallback(std::function<void(bool)> callback)
	{
		m_onMenuStateChanged = callback;
	}

	/// @brief メニューを開く/閉じる
	void ToggleMenu();
	void OpenMenu();
	void CloseMenu();
	bool IsOpen() const { return m_isOpen; }

	/// @brief 更新処理
	void Update();

	/// @brief 描画処理
	void Draw() const;

	/// @brief メニューサイズを設定
	void SetSize(int width, int height);
	void SetPosition(const s3d::Point& pos);

private:
	/// @brief UI要素の位置を計算
	void CalculateLayout();

	/// @brief スロットの描画
	void DrawSlots() const;
	void DrawSlot(int slotIndex, const s3d::Rect& slotRect, const InventoryItem* item) const;

	/// @brief ヘッダーの描画
	void DrawHeader() const;

	/// @brief ボタンの描画
	void DrawButtons() const;
	void DrawButton(const s3d::Rect& rect, const s3d::String& text, bool isHovered, const s3d::ColorF& color) const;

	/// @brief アイテム情報の描画
	void DrawItemInfo() const;

	/// @brief ツールチップの描画
	void DrawTooltip(const InventoryItem* item, const s3d::Point& pos) const;

	/// @brief ドラッグ中のアイテム描画
	void DrawDraggedItem() const;

	/// @brief エラーメッセージ描画
	void DrawErrorMessage() const;

	/// @brief 入力処理
	void HandleInput();
	void HandleSlotClick(int slotIndex);

	/// @brief スロットインデックスを座標から取得
	int GetSlotIndexFromPosition(const s3d::Point& pos) const;

	/// @brief アニメーション更新
	void UpdateAnimation();

	/// @brief 売却処理
	void SellSelectedItem();

	mutable std::vector<InventoryItem*> m_displayedItems;
};
