#pragma once

class GameObject;

class GameObjectContainer
{
private:
	GameObject** m_objects{ nullptr };
	size_t m_count{ 0 };
	size_t m_capacity{ 0 };

public:
	GameObjectContainer() = default;
	GameObjectContainer(size_t capacity)
		: m_objects(new GameObject* [capacity] {}), m_count(0), m_capacity(capacity) {
	}

	~GameObjectContainer() {
		delete[] m_objects;
	}

	GameObjectContainer& operator=(const GameObjectContainer& container) {
		if (this == &container) return *this;

		delete[] m_objects;

		m_count = container.m_count;
		m_capacity = container.m_capacity;

		m_objects = (m_capacity > 0) ? new GameObject * [m_capacity] {} : nullptr;
		for (size_t i = 0; i < m_count; ++i) {
			m_objects[i] = container.m_objects[i];
		}

		return *this;
	}

	GameObjectContainer(const GameObjectContainer& container) {
		*this = container;
	}

	size_t GetCount() const { return m_count; }

	GameObject* GetGameObject(unsigned long long index) {
		assert(m_objects);
		return m_objects[index];
	}

	void Registar(GameObject* object) {
		assert(m_count < m_capacity);
		m_objects[m_count++] = object;
	}
};
