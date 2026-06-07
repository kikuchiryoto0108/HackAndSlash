#pragma once
//=======================
//
//	GameObject.h
// 
//	2025/11/28
// 
//=======================
#include "Collision.h"

class GameWorld;

class GameObject
{
private:
	Vec2 m_position{ 0.0, 0.0 };
	GameWorld* m_owner{ nullptr };
	std::string m_tag;
	bool m_is_destroy{ false };

public:
	GameObject(GameWorld* owner, const std::string& tag);
	virtual ~GameObject() = default;

protected:
	GameWorld* GetOwner() const { return m_owner; }
	void SetPosition(const Vec2& position) { m_position = position; }

public:
	void Destroy() { m_is_destroy = true; }
	const std::string& GetTag() const { return m_tag; }
	const Vec2& GetPosition() const { return m_position; }
	bool IsDestroy() const { return m_is_destroy; }
	virtual void Damage(int) {}
	virtual MyCircle GetCollision() const { return { {}, 0.0f }; }

	virtual void Update() = 0;
	virtual void UpdateUI() {}
	virtual void Draw() const = 0;

};



