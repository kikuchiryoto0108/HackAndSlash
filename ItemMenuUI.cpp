#include "stdafx.h"
#include "ItemMenuUI.h"
#include "Player.h"
#include <unordered_set>

ItemMenuUI::ItemMenuUI(Inventory* inventory, Player* player)
    : m_inventory(inventory), m_player(player)
{
    SetSize(1200, 700);
    SetPosition(s3d::Point(200, 100));
    CalculateLayout();
}

void ItemMenuUI::ToggleMenu()
{
    if (m_isOpen)
    {
        CloseMenu();
    }
    else
    {
        OpenMenu();
    }
}

void ItemMenuUI::OpenMenu()
{
    if (!m_isOpen)
    {
        m_isOpen = true;
        m_targetAlpha = 1.0;
        m_currentAlpha = 0.5; // ★追加: 初期値を0.5にして即座に描画開始
        m_openAnimation.restart();
    }
}

void ItemMenuUI::CloseMenu()
{
	if (m_isOpen)
	{
		m_isOpen = false;
		m_targetAlpha = 0.0;
		m_selectedSlot = -1;
		m_isDragging = false;
		if (m_onMenuStateChanged)
		{
			m_onMenuStateChanged(false);  // false = 閉じた
		}
	}
}

void ItemMenuUI::Update()
{
    if (!m_isOpen && m_currentAlpha <= 0.0) return;
    
    UpdateAnimation();
    
    // エラーメッセージのタイマー更新
    if (m_errorMessageTime > 0.0)
    {
        m_errorMessageTime -= s3d::Scene::DeltaTime();
        if (m_errorMessageTime < 0.0)
        {
            m_errorMessageTime = 0.0;
            m_errorMessage.clear();
        }
    }
    
    if (m_isOpen)
    {
        HandleInput();
        
        // ホバースロットの更新
        m_hoveredSlot = GetSlotIndexFromPosition(s3d::Cursor::Pos());
    }
}

void ItemMenuUI::Draw() const
{
    if (m_currentAlpha <= 0.0) return;
    
    const s3d::ScopedRenderStates2D blend{ s3d::BlendState::Default2D };
    
    // 背景
    m_menuRect.draw(s3d::ColorF(m_style.backgroundColor.r, m_style.backgroundColor.g, m_style.backgroundColor.b, m_currentAlpha * m_style.backgroundColor.a));
    m_menuRect.drawFrame(2, s3d::ColorF(m_style.borderColor, m_currentAlpha));
    
    // 修正: 0.5 → 0.1に変更して早めに詳細を描画
    if (m_currentAlpha > 0.1)
    {
        DrawHeader();
        DrawSlots();
        DrawButtons();
        DrawItemInfo();
        
        // ドラッグ中のアイテム（最前面）
        if (m_isDragging)
        {
            DrawDraggedItem();
        }
        
        // エラーメッセージ
        DrawErrorMessage();
    }
}

void ItemMenuUI::CalculateLayout()
{
    m_menuRect = s3d::Rect(m_menuPos, 1200, 700);
    
    // ヘッダー
    m_headerRect = s3d::Rect(m_menuPos.x, m_menuPos.y, m_menuRect.w, m_style.headerHeight);
    
    // スロットエリアの幅を計算（10スロット分）
    double slotsAreaWidth = (m_style.slotSize + m_style.slotSpacing) * m_style.slotsPerRow - m_style.slotSpacing;
    
    // スロットエリア（左側、5行分）
    m_slotsRect = s3d::Rect(
        m_menuPos.x + m_style.margin,
        m_menuPos.y + m_style.headerHeight + m_style.margin,
        slotsAreaWidth,
        (m_style.slotSize + m_style.slotSpacing) * 5 - m_style.slotSpacing
    );
    
    // 情報エリア（右側）
    m_infoRect = s3d::Rect(
        m_slotsRect.x + m_slotsRect.w + m_style.margin,
        m_slotsRect.y,
        m_style.infoPanelWidth,
        m_slotsRect.h
    );
    
    // ボタンエリア（スロットの下）
    m_buttonRect = s3d::Rect(
        m_menuPos.x + m_style.margin,
        m_slotsRect.y + m_slotsRect.h + m_style.margin,
        m_menuRect.w - m_style.margin * 2,
        m_style.buttonAreaHeight
    );
    
    // ボタンの位置を計算
    m_sortButtons.clear();
    m_filterButtons.clear();
    
    const std::vector<s3d::String> sortLabels = {
        U"名前", U"種類", U"レア度", U"数量", U"取得順"
    };
    
    double buttonWidth = 85.0;
    for (size_t i = 0; i < sortLabels.size(); ++i)
    {
        m_sortButtons.push_back(s3d::Rect(
            m_buttonRect.x + (buttonWidth + 5) * i,
            m_buttonRect.y + 5,
            buttonWidth,
            m_style.buttonHeight
        ));
    }
    
    // フィルターボタン
    const std::vector<s3d::String> filterLabels = {
        U"消耗品", U"武器", U"防具", U"素材", U"宝物", U"装備中"
    };
    
    double filterStartX = m_buttonRect.x + (buttonWidth + 5) * sortLabels.size() + 20;
    for (size_t i = 0; i < filterLabels.size(); ++i)
    {
        m_filterButtons.push_back(s3d::Rect(
            filterStartX + (buttonWidth + 5) * i,
            m_buttonRect.y + 5,
            buttonWidth,
            m_style.buttonHeight
        ));
    }
    
    // ソート順序切り替えボタン
    m_sortOrderButton = s3d::Rect(
        m_buttonRect.x,
        m_buttonRect.y + m_style.buttonHeight + 10,
        90,
        m_style.buttonHeight
    );
    
    // 売却ボタン
    m_sellButton = s3d::Rect(
        m_buttonRect.x + m_buttonRect.w - 310,
        m_buttonRect.y + m_style.buttonHeight + 10,
        95,
        m_style.buttonHeight
    );
    
    // 一括売却ボタン
    m_sellAllButton = s3d::Rect(
        m_buttonRect.x + m_buttonRect.w - 205,
        m_buttonRect.y + m_style.buttonHeight + 10,
        95,
        m_style.buttonHeight
    );
    
    // 閉じるボタン
    m_closeButton = s3d::Rect(
        m_buttonRect.x + m_buttonRect.w - 100,
        m_buttonRect.y + m_style.buttonHeight + 10,
        95,
        m_style.buttonHeight
    );
}

void ItemMenuUI::DrawHeader() const
{
    m_headerRect.draw(s3d::ColorF(0.15, 0.15, 0.25));
    m_headerRect.drawFrame(2, m_style.borderColor);
    
    s3d::FontAsset::Register(U"MenuTitle", 28, s3d::Typeface::Bold);
    s3d::FontAsset(U"MenuTitle")(U"アイテム・インベントリ")
        .draw(m_headerRect.x + 30, m_headerRect.y + 15, s3d::ColorF(0.9, 0.9, 1.0));
    
    // 統計情報
    if (m_inventory)
    {
        s3d::FontAsset::Register(U"MenuInfo", 18);
        
        // スロット数表示
        s3d::String slotInfo = s3d::Format(U"スロット: {}/{}", m_inventory->GetItemCount(), m_inventory->GetMaxSlots());
        s3d::FontAsset(U"MenuInfo")(slotInfo)
            .draw(m_headerRect.x + m_headerRect.w - 500, m_headerRect.y + 15, m_style.textColor);
        
        // 所持金表示
        if (m_player)
        {
            s3d::String moneyInfo = s3d::Format(U"所持金: {} G", m_player->GetMoney());
            s3d::FontAsset(U"MenuInfo")(moneyInfo)
                .draw(m_headerRect.x + m_headerRect.w - 500, m_headerRect.y + 45, s3d::ColorF(1.0, 0.9, 0.2));
        }
    }
}

void ItemMenuUI::DrawSlots() const
{
	if (!m_inventory) return;

	// フィルター適用されたアイテムを取得
	m_displayedItems = m_inventory->GetFilteredItems();

	// 表示するスロット数を計算（5行まで表示）
	int maxVisibleSlots = m_style.slotsPerRow * 5; // 10列 x 5行 = 50スロット
	int slotsToDisplay = s3d::Min(static_cast<int>(m_displayedItems.size()), maxVisibleSlots);

	// フィルター適用されたアイテムを描画
	for (int i = 0; i < slotsToDisplay; ++i)
	{
		int row = i / m_style.slotsPerRow;
		int col = i % m_style.slotsPerRow;

		s3d::Rect slotRect(
			m_slotsRect.x + col * (m_style.slotSize + m_style.slotSpacing),
			m_slotsRect.y + row * (m_style.slotSize + m_style.slotSpacing),
			m_style.slotSize,
			m_style.slotSize
		);

		InventoryItem* item = m_displayedItems[i];
		DrawSlot(i, slotRect, item);
	}

	// 残りの空スロットを描画
	for (int i = slotsToDisplay; i < maxVisibleSlots; ++i)
	{
		int row = i / m_style.slotsPerRow;
		int col = i % m_style.slotsPerRow;

		s3d::Rect slotRect(
			m_slotsRect.x + col * (m_style.slotSize + m_style.slotSpacing),
			m_slotsRect.y + row * (m_style.slotSize + m_style.slotSpacing),
			m_style.slotSize,
			m_style.slotSize
		);

		// 空スロット描画
		slotRect.draw(m_style.slotColor);
		slotRect.drawFrame(1, m_style.borderColor);
	}
}

void ItemMenuUI::DrawSlot(int slotIndex, const s3d::Rect& slotRect, const InventoryItem* item) const
{
    // スロットの背景色を決定
    s3d::ColorF slotColor = m_style.slotColor;
    
    if (slotIndex == m_selectedSlot)
    {
        slotColor = m_style.slotSelectedColor;
    }
    else if (slotIndex == m_hoveredSlot)
    {
        slotColor = m_style.slotHoverColor;
    }
    
    // スロットの描画
    slotRect.draw(slotColor);
    slotRect.drawFrame(1, m_style.borderColor);
    
    if (item && item->itemData)
    {
        // レア度に応じた枠線（太さ3で目立たせる）
        slotRect.drawFrame(3, item->itemData->GetRarityColor());
        
        // アイテムアイコン（タイプカラーの円）
        s3d::Circle iconCircle(slotRect.center(), slotRect.w * 0.25);
        iconCircle.draw(item->itemData->GetTypeColor());
        
        // レア度カラーの外周
        s3d::Circle(slotRect.center(), slotRect.w * 0.28).drawFrame(2, item->itemData->GetRarityColor());
        
        // アイテム名の短縮表示（下部に配置）
        s3d::FontAsset::Register(U"SlotText", 11);
        s3d::String shortName = item->itemData->name;
        if (shortName.length() > 7) {
            shortName = shortName.substr(0, 6) + U"…";
        }
        
        // 背景をつけて読みやすく
        s3d::RectF textBg = s3d::FontAsset(U"SlotText")(shortName).region(slotRect.center().x, slotRect.y + slotRect.h - 16);
        textBg.x -= textBg.w / 2;
        s3d::RectF(textBg.x - 2, textBg.y - 1, textBg.w + 4, textBg.h + 2).draw(s3d::ColorF(0.0, 0.0, 0.0, 0.7));
        
        s3d::FontAsset(U"SlotText")(shortName)
            .drawAt(slotRect.center().x, slotRect.y + slotRect.h - 16, m_style.textColor);
        
        // 数量表示（左上）
        if (item->quantity > 1)
        {
            s3d::String quantityStr = s3d::Format(U"x{}", item->quantity);
            s3d::FontAsset::Register(U"SlotQuantity", 14, s3d::Typeface::Bold);
            
            // 数量の背景
            s3d::RectF qtyBg = s3d::FontAsset(U"SlotQuantity")(quantityStr).region(slotRect.x + 5, slotRect.y + 5);
            s3d::RectF(qtyBg.x - 2, qtyBg.y - 1, qtyBg.w + 4, qtyBg.h + 2).draw(s3d::ColorF(0.0, 0.0, 0.0, 0.8));
            
            s3d::FontAsset(U"SlotQuantity")(quantityStr)
                .draw(slotRect.x + 5, slotRect.y + 5, s3d::ColorF(1.0, 1.0, 0.2));
        }
        
        // 装備中マーク（右上）
        if (item->isEquipped)
        {
            s3d::Rect equipMark(slotRect.x + slotRect.w - 18, slotRect.y + 3, 15, 15);
            equipMark.draw(s3d::ColorF(0.2, 1.0, 0.2));
            equipMark.drawFrame(1, s3d::ColorF(0.0, 0.5, 0.0));
            s3d::FontAsset::Register(U"EquipMark", 11, s3d::Typeface::Bold);
            s3d::FontAsset(U"EquipMark")(U"E").drawAt(equipMark.center(), s3d::ColorF(0.0, 0.0, 0.0));
        }
        
        // ホバー時のツールチップ
        if (slotIndex == m_hoveredSlot && !m_isDragging)
        {
            DrawTooltip(item, s3d::Cursor::Pos());
        }
    }
}

void ItemMenuUI::DrawButtons() const
{
    const std::vector<s3d::String> sortLabels = {
        U"名前", U"種類", U"レア度", U"数量", U"取得順"
    };
    
    s3d::FontAsset::Register(U"ButtonLabel", 12);
    
    // ソートラベル
    s3d::FontAsset(U"ButtonLabel")(U"並び替え:")
        .draw(m_buttonRect.x, m_buttonRect.y - 18, m_style.textColor);
    
    // ソートボタン
    for (size_t i = 0; i < m_sortButtons.size() && i < sortLabels.size(); ++i)
    {
        bool isHovered = m_sortButtons[i].contains(s3d::Cursor::Pos());
        bool isActive = (static_cast<int>(m_lastSortType) == i + 1);
        s3d::ColorF btnColor = isActive ? s3d::ColorF(0.5, 0.5, 0.8) : m_style.buttonColor;
        DrawButton(m_sortButtons[i], sortLabels[i], isHovered, btnColor);
    }
    
    const std::vector<s3d::String> filterLabels = {
        U"消耗品", U"武器", U"防具", U"素材", U"宝物", U"装備中"
    };
    
    // フィルターラベル
    s3d::FontAsset(U"ButtonLabel")(U"フィルター:")
        .draw(m_sortButtons[0].x + (m_sortButtons[0].w + 5) * sortLabels.size() + 20, 
              m_buttonRect.y - 18, m_style.textColor);
    
    // フィルターボタン
    for (size_t i = 0; i < m_filterButtons.size() && i < filterLabels.size(); ++i)
    {
        bool isHovered = m_filterButtons[i].contains(s3d::Cursor::Pos());
        bool isActive = false;
        
        // フィルターがアクティブかチェック
        if (i < 5 && m_currentFilter.useTypeFilter && static_cast<int>(m_currentFilter.typeFilter) == i) 
            isActive = true;
        if (i == 5 && m_currentFilter.showEquippedOnly) 
            isActive = true;
            
        s3d::ColorF btnColor = isActive ? s3d::ColorF(0.5, 0.8, 0.5) : m_style.buttonColor;
        DrawButton(m_filterButtons[i], filterLabels[i], isHovered, btnColor);
    }
    
    // ソート順序ボタン
    bool sortOrderHovered = m_sortOrderButton.contains(s3d::Cursor::Pos());
    bool hasSortApplied = m_inventory && m_inventory->GetCurrentSort() != SortType::None;
    s3d::String sortOrderText = hasSortApplied
        ? (m_inventory->IsSortAscending() ? U"▲ 昇順" : U"▼ 降順")
        : U"並び順";
    DrawButton(m_sortOrderButton, sortOrderText, sortOrderHovered, s3d::ColorF(0.4, 0.4, 0.5));
    
    // 売却ボタン
    bool sellHovered = m_sellButton.contains(s3d::Cursor::Pos());
    bool canSell = m_selectedSlot >= 0 && m_inventory && m_inventory->GetItem(m_selectedSlot);
    s3d::ColorF sellColor = canSell ? s3d::ColorF(0.8, 0.3, 0.3) : s3d::ColorF(0.4, 0.2, 0.2);
    DrawButton(m_sellButton, U"売却", sellHovered, sellColor);
    
    // 一括売却ボタン
    bool sellAllHovered = m_sellAllButton.contains(s3d::Cursor::Pos());
    DrawButton(m_sellAllButton, U"一括売却", sellAllHovered, s3d::ColorF(0.7, 0.2, 0.2));
    
    // 閉じるボタン
    bool closeHovered = m_closeButton.contains(s3d::Cursor::Pos());
    DrawButton(m_closeButton, U"閉じる", closeHovered, s3d::ColorF(0.6, 0.3, 0.3));
}

void ItemMenuUI::DrawButton(const s3d::Rect& rect, const s3d::String& text, bool isHovered, const s3d::ColorF& color) const
{
    s3d::ColorF buttonColor = isHovered ? m_style.buttonHoverColor : color;
    rect.draw(buttonColor);
    rect.drawFrame(1, m_style.borderColor);
    
    s3d::FontAsset::Register(U"ButtonFont", 14);
    s3d::FontAsset(U"ButtonFont")(text).drawAt(rect.center(), m_style.textColor);
}

void ItemMenuUI::DrawItemInfo() const
{
    // 情報パネルの背景
    m_infoRect.draw(s3d::ColorF(0.12, 0.12, 0.22, 0.95));
    m_infoRect.drawFrame(2, m_style.borderColor);
    
    if (!m_inventory || m_selectedSlot < 0) 
    {
        // アイテム未選択時のガイド
        s3d::FontAsset::Register(U"InfoHint", 16);
        s3d::FontAsset(U"InfoHint")(U"アイテムを選択してください")
            .drawAt(m_infoRect.center(), s3d::ColorF(0.6, 0.6, 0.6));
        return;
    }
    
    InventoryItem* item = m_inventory->GetItem(m_selectedSlot);
    if (!item || !item->itemData) return;
    
    s3d::FontAsset::Register(U"InfoTitle", 20, s3d::Typeface::Bold);
    s3d::FontAsset::Register(U"InfoText", 16);
    s3d::FontAsset::Register(U"InfoSmall", 14);
    
    int y = m_infoRect.y + 15;
    int x = m_infoRect.x + 15;
    
    // アイテム名
    s3d::String itemName = item->itemData->name;
    s3d::FontAsset(U"InfoTitle")(itemName)
        .draw(x, y, item->itemData->GetRarityColor());
    y += 30;
    
    // レア度表示
    const std::vector<s3d::String> rarityNames = {
        U"一般", U"非一般", U"レア", U"エピック", U"伝説"
    };
    if (static_cast<int>(item->itemData->rarity) < rarityNames.size())
    {
        s3d::FontAsset(U"InfoSmall")(U"レア度: " + rarityNames[static_cast<int>(item->itemData->rarity)])
            .draw(x, y, item->itemData->GetRarityColor());
        y += 22;
    }
    
    // タイプ表示
    const std::vector<s3d::String> typeNames = {
        U"消耗品", U"武器", U"防具", U"素材", U"宝物", U"クエスト"
    };
    if (static_cast<int>(item->itemData->type) < typeNames.size())
    {
        s3d::FontAsset(U"InfoSmall")(U"種類: " + typeNames[static_cast<int>(item->itemData->type)])
            .draw(x, y, item->itemData->GetTypeColor());
        y += 25;
    }
    
    // 区切り線
    s3d::Line(x, y, x + m_infoRect.w - 30, y).draw(s3d::ColorF(0.4, 0.4, 0.5));
    y += 15;
    
    // 説明
    s3d::String description = item->itemData->description;
    s3d::FontAsset(U"InfoText")(description)
        .draw(s3d::Rect(x, y, m_infoRect.w - 30, 100), m_style.textColor);
    y += 110;
    
    // 区切り線
    s3d::Line(x, y, x + m_infoRect.w - 30, y).draw(s3d::ColorF(0.4, 0.4, 0.5));
    y += 15;
    
    // 詳細情報
    s3d::FontAsset(U"InfoText")(U"所持数: {}"_fmt(item->quantity))
        .draw(x, y, m_style.textColor);
    y += 25;
    
    s3d::FontAsset(U"InfoText")(U"売却価格: {} G"_fmt(item->itemData->sellPrice))
        .draw(x, y, s3d::ColorF(1.0, 0.9, 0.2));
    y += 25;
    
    if (item->quantity > 1)
    {
        int totalValue = item->itemData->sellPrice * item->quantity;
        s3d::FontAsset(U"InfoText")(U"合計売却額: {} G"_fmt(totalValue))
            .draw(x, y, s3d::ColorF(1.0, 0.8, 0.1));
        y += 25;
    }
    
    if (item->isEquipped)
    {
        s3d::FontAsset(U"InfoText")(U"[装備中]")
            .draw(x, y, s3d::ColorF(0.3, 1.0, 0.3));
        y += 25;
    }
    
    // 操作ヒント
    y = m_infoRect.y + m_infoRect.h - 80;
    s3d::Line(x, y, x + m_infoRect.w - 30, y).draw(s3d::ColorF(0.4, 0.4, 0.5));
    y += 10;
    
    s3d::FontAsset(U"InfoSmall")(U"右クリック: 使用")
        .draw(x, y, s3d::ColorF(0.7, 0.7, 0.7));
    y += 20;
    s3d::FontAsset(U"InfoSmall")(U"Shift+クリック: 装備")
        .draw(x, y, s3d::ColorF(0.7, 0.7, 0.7));
    y += 20;
    s3d::FontAsset(U"InfoSmall")(U"ドラッグ: 並び替え")
        .draw(x, y, s3d::ColorF(0.7, 0.7, 0.7));
}

void ItemMenuUI::DrawTooltip(const InventoryItem* item, const s3d::Point& pos) const
{
    if (!item || !item->itemData) return;
    
    s3d::String tooltipText = item->itemData->name;
    s3d::FontAsset::Register(U"Tooltip", 14);
    
    s3d::RectF textRect = s3d::FontAsset(U"Tooltip")(tooltipText).region();
    int tooltipWidth = static_cast<int>(textRect.w) + 20;
    int tooltipHeight = static_cast<int>(textRect.h) + 20;
    
    // ツールチップがメニューの外に出ないように調整
    int tooltipX = pos.x + 15;
    int tooltipY = pos.y - 5;
    
    // 右端チェック
    if (tooltipX + tooltipWidth > m_menuRect.x + m_menuRect.w)
    {
        tooltipX = pos.x - tooltipWidth - 15;
    }
    
    // 下端チェック
    if (tooltipY + tooltipHeight > m_menuRect.y + m_menuRect.h)
    {
        tooltipY = pos.y - tooltipHeight - 5;
    }
    
    s3d::Rect tooltipRect(tooltipX, tooltipY, tooltipWidth, tooltipHeight);
    
    // 背景と枠線
    tooltipRect.draw(s3d::ColorF(0.05, 0.05, 0.1, 0.95));
    tooltipRect.drawFrame(2, item->itemData->GetRarityColor());
    
    // テキスト描画
    s3d::FontAsset(U"Tooltip")(tooltipText)
        .draw(tooltipRect.x + 10, tooltipRect.y + 10, item->itemData->GetRarityColor());
}

void ItemMenuUI::DrawDraggedItem() const
{
    if (!m_inventory || m_draggedSlot < 0) return;
    
    InventoryItem* item = m_inventory->GetItem(m_draggedSlot);
    if (!item || !item->itemData) return;
    
    s3d::Point dragPos = s3d::Cursor::Pos() + m_dragOffset;
    s3d::Rect dragRect(dragPos, m_style.slotSize, m_style.slotSize);
    
    dragRect.draw(s3d::ColorF(item->itemData->GetRarityColor(), 0.8));
    
    s3d::Circle iconCircle(dragRect.center(), dragRect.w * 0.3);
    iconCircle.draw(s3d::ColorF(item->itemData->GetTypeColor(), 0.8));
}

void ItemMenuUI::DrawErrorMessage() const
{
    if (m_errorMessage.isEmpty() || m_errorMessageTime <= 0.0) return;
    
    // エラーメッセージを画面中央下部に表示
    s3d::FontAsset::Register(U"ErrorFont", 20, s3d::Typeface::Bold);
    
    s3d::Vec2 messagePos(m_menuRect.x + m_menuRect.w / 2, m_menuRect.y + m_menuRect.h - 100);
    
    // 背景
    s3d::RectF textRegion = s3d::FontAsset(U"ErrorFont")(m_errorMessage).regionAt(messagePos);
    s3d::RectF bgRect(textRegion.x - 20, textRegion.y - 10, textRegion.w + 40, textRegion.h + 20);
    
    // フェードアウト効果
    double alpha = s3d::Min(m_errorMessageTime / 0.5, 1.0);
    
    bgRect.draw(s3d::ColorF(0.8, 0.2, 0.2, 0.9 * alpha));
    bgRect.drawFrame(2, s3d::ColorF(1.0, 0.5, 0.5, alpha));
    
    s3d::FontAsset(U"ErrorFont")(m_errorMessage)
        .drawAt(messagePos, s3d::ColorF(1.0, 1.0, 1.0, alpha));
}

void ItemMenuUI::HandleInput()
{
    // キーボードショートカット
    // 修正: Iキーの処理を削除（Player側で処理）、ESCキーのみ残す
    if (s3d::KeyEscape.down())
    {
        CloseMenu();
        return;
    }
    
    // マウス入力
    if (s3d::MouseL.down())
    {
        s3d::Point mousePos = s3d::Cursor::Pos();
        
        // 閉じるボタン
        if (m_closeButton.contains(mousePos))
        {
            CloseMenu();
            return;
        }
        
        // 売却ボタン
        if (m_sellButton.contains(mousePos))
        {
            SellSelectedItem();
            return;
        }
        
        // 一括売却ボタン
        if (m_sellAllButton.contains(mousePos))
        {
            if (m_inventory)
            {
                // フィルター適用されたアイテムを先に取得
                auto filteredItems = m_inventory->GetFilteredItems();
                std::unordered_set<const InventoryItem*> filteredSet(filteredItems.begin(), filteredItems.end());
                
                // アイテムIDと数量を収集（削除中のイテレータ無効化を避けるため）
                std::vector<std::pair<int, int>> itemsToSell;
                int totalMoney = 0;
                
                for (size_t i = 0; i < m_inventory->GetItemCount(); ++i)
                {
                    InventoryItem* item = m_inventory->GetItem(i);
                    if (item && item->itemData && !item->isEquipped)
                    {
                        // フィルターセットで高速チェック
                        if (filteredSet.find(item) != filteredSet.end())
                        {
                            itemsToSell.push_back({item->itemData->id, item->quantity});
                            totalMoney += item->itemData->sellPrice * item->quantity;
                        }
                    }
                }
                
                // 収集したアイテムを削除
                for (const auto& itemPair : itemsToSell)
                {
                    m_inventory->RemoveItem(itemPair.first, itemPair.second);
                }
                
                if (m_player && totalMoney > 0)
                {
                    m_player->AddMoney(totalMoney);
                }
                
                m_selectedSlot = -1;
            }
            return;
        }
        
        // ソート順序切り替えボタン
        if (m_sortOrderButton.contains(mousePos))
        {
            if (m_inventory && m_lastSortType != SortType::None)
            {
                m_inventory->SortItems(m_lastSortType);
            }
            return;
        }
        
        // ソートボタン
        for (size_t i = 0; i < m_sortButtons.size(); ++i)
        {
            if (m_sortButtons[i].contains(mousePos))
            {
                SortType sortType = static_cast<SortType>(i + 1);
                m_lastSortType = sortType;
                m_inventory->SortItems(sortType);
                return;
            }
        }
        
        // フィルターボタン
        for (size_t i = 0; i < m_filterButtons.size(); ++i)
        {
            if (m_filterButtons[i].contains(mousePos))
            {
                FilterSettings filter = m_currentFilter;
                
                if (i < 5) // タイプフィルター
                {
                    ItemType filterType = static_cast<ItemType>(i);
                    if (filter.useTypeFilter && filter.typeFilter == filterType)
                    {
                        // 同じフィルターをクリックしたらオフ
                        filter.useTypeFilter = false;
                    }
                    else
                    {
                        filter.useTypeFilter = true;
                        filter.typeFilter = filterType;
                    }
                }
                else if (i == 5) // 装備中フィルター
                {
                    filter.showEquippedOnly = !filter.showEquippedOnly;
                }
                
                m_currentFilter = filter;
                if (m_inventory)
                {
                    m_inventory->SetFilter(filter);
                }
                return;
            }
        }
        
        // スロットクリック
        int slotIndex = GetSlotIndexFromPosition(mousePos);
        if (slotIndex >= 0)
        {
            HandleSlotClick(slotIndex);
        }
    }
    
    // 右クリックでアイテム使用
    if (s3d::MouseR.down())
    {
        int slotIndex = GetSlotIndexFromPosition(s3d::Cursor::Pos());
        if (slotIndex >= 0 && m_inventory)
        {
            m_inventory->UseItem(slotIndex);
        }
    }
    
    // ドラッグ終了
    if (m_isDragging && s3d::MouseL.up())
    {
        int dropSlot = GetSlotIndexFromPosition(s3d::Cursor::Pos());
        if (dropSlot >= 0 && dropSlot != m_draggedSlot && m_inventory)
        {
            // アイテムの入れ替え処理（将来実装）
            // TODO: Implement item swapping between slots
            // 現在は単にドラッグをキャンセル
        }
        m_isDragging = false;
        m_draggedSlot = -1;
    }
}

void ItemMenuUI::HandleSlotClick(int slotIndex)
{
    if (!m_inventory) return;
    
    if (s3d::KeyShift.pressed())
    {
    // Shift+クリックで装備切り替え
m_inventory->ToggleEquip(slotIndex);
    }
    else
    {
   // 通常クリックで選択
        m_selectedSlot = (m_selectedSlot == slotIndex) ? -1 : slotIndex;
  
        // ドラッグ開始
        InventoryItem* item = m_inventory->GetItem(slotIndex);
if (item)
    {
        m_isDragging = true;
        m_draggedSlot = slotIndex;
         
int row = slotIndex / m_style.slotsPerRow;
      int col = slotIndex % m_style.slotsPerRow;
    s3d::Vec2 slotPos(
      m_slotsRect.x + col * (m_style.slotSize + m_style.slotSpacing),
       m_slotsRect.y + row * (m_style.slotSize + m_style.slotSpacing)
   );
 
    s3d::Point cursorPos = s3d::Cursor::Pos();
   m_dragOffset = s3d::Point(static_cast<int>(slotPos.x) - cursorPos.x, static_cast<int>(slotPos.y) - cursorPos.y);
  }
    }
}

void ItemMenuUI::SetPosition(const s3d::Point& pos)
{
    m_menuPos = pos;
    m_menuRect.pos = pos;
    CalculateLayout();
}

int ItemMenuUI::GetSlotIndexFromPosition(const s3d::Point& pos) const
{
	if (!m_slotsRect.contains(pos)) return -1;

	int col = static_cast<int>((pos.x - m_slotsRect.x) / (m_style.slotSize + m_style.slotSpacing));
	int row = static_cast<int>((pos.y - m_slotsRect.y) / (m_style.slotSize + m_style.slotSpacing));

	if (col < 0 || col >= m_style.slotsPerRow) return -1;

	int displayIndex = row * m_style.slotsPerRow + col;

	// 表示されているアイテムのインデックスを取得
	if (displayIndex >= 0 && displayIndex < static_cast<int>(m_displayedItems.size()))
	{
		InventoryItem* clickedItem = m_displayedItems[displayIndex];

		// 実際のインベントリ内でのインデックスを検索
		for (size_t i = 0; i < m_inventory->GetItemCount(); ++i)
		{
			if (m_inventory->GetItem(i) == clickedItem)
			{
				return static_cast<int>(i);
			}
		}
	}

	return -1;
}

void ItemMenuUI::UpdateAnimation()
{
    const double animSpeed = 10.0; // 5.0 → 10.0に変更（2倍の速度）
    double deltaTime = s3d::Scene::DeltaTime();
    
    if (s3d::Math::Abs(m_currentAlpha - m_targetAlpha) > 0.01)
    {
        m_currentAlpha = s3d::Math::Lerp(m_currentAlpha, m_targetAlpha, animSpeed * deltaTime);
    }
    else
    {
        m_currentAlpha = m_targetAlpha;
    }
}

void ItemMenuUI::SellSelectedItem()
{
    if (!m_inventory || m_selectedSlot < 0) return;
    
    InventoryItem* item = m_inventory->GetItem(m_selectedSlot);
    if (!item || !item->itemData) return;
    
    // 装備中のアイテムは売却できない
    if (item->isEquipped)
    {
        m_errorMessage = U"装備中のアイテムは売却できません";
        m_errorMessageTime = 2.0; // 2秒間表示
        return;
    }
    
    // 売却処理
    int sellPrice = item->itemData->sellPrice * item->quantity;
    m_inventory->RemoveItem(item->itemData->id, item->quantity);
    
    // プレイヤーの所持金に追加
    if (m_player)
    {
        m_player->AddMoney(sellPrice);
    }
    
    m_selectedSlot = -1;
}

void ItemMenuUI::SetSize(int width, int height)
{
    // 1600x900ウィンドウ用の推奨サイズを定数として定義
    static constexpr int RECOMMENDED_WIDTH = 1200;
    static constexpr int RECOMMENDED_HEIGHT = 700;
    
    // 指定されたサイズを尊重しつつ、推奨サイズに近づける
    width = (width > 0) ? width : RECOMMENDED_WIDTH;
    height = (height > 0) ? height : RECOMMENDED_HEIGHT;
    
    m_menuRect.w = width;
    m_menuRect.h = height;
    CalculateLayout();
}
