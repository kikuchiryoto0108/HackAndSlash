#pragma once

#include "GameObject.h"

/// @brief インタラクト可能な宝箱オブジェクト
class TreasureChest : public GameObject
{
private:
    static constexpr double RADIUS = 40.0;
    bool m_isOpened = false;
    bool m_isRareChest = false;
    s3d::Stopwatch m_openAnimation;
    double m_interactionRange = 80.0;
    
public:
    TreasureChest(GameWorld* owner, const Vec2& position, bool isRare = false);
    
    void Update() override;
    void Draw() const override;
    
    MyCircle GetCollision() const { return { GetPosition(), RADIUS }; }
    
    /// @brief プレイヤーが近くにいるか判定
    bool IsPlayerNearby() const;
    
    /// @brief 宝箱を開く
    void OpenChest();
    
    bool IsOpened() const { return m_isOpened; }

private:
    void DrawInteractionPrompt() const;
};
