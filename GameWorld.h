#pragma once

class GameObject;
class MyCircle;
class GameObjectContainer;

#include "Collision.h"

class GameWorld
{
private:
	static constexpr size_t MAX_OBJECTS = 100;
	GameObject* m_objects[MAX_OBJECTS]{};
	size_t m_objects_count{ 0 };

	// インベントリポーズ用
	bool m_isPaused = false;

public:
	~GameWorld();

	GameObject* Registar(GameObject* object){
		assert(m_objects_count < MAX_OBJECTS);
		m_objects[m_objects_count++] = object;
		return object;
	}

	GameObjectContainer GetGameObjects(const std::string& tag);
	GameObjectContainer GetGameObjects(const MyCircle& collision);

	void SetPaused(bool paused) { m_isPaused = paused; }
	bool IsPaused() const { return m_isPaused; }

	void Update();
	void UpdateUI();
	void Draw() const;
};
