#include "stdafx.h"
#include "GameWorld.h"
#include "GameObjectContainer.h"
#include "GameObject.h"
#include "Collision.h"

GameWorld::~GameWorld()
{
	for (size_t i = 0; i < m_objects_count; i++)
	{
		delete m_objects[i];
	}
}

GameObjectContainer GameWorld::GetGameObjects(const std::string& tag)
{
	size_t count{ 0 };

	for (size_t i = 0; i < m_objects_count; i++) {
		if (m_objects[i]->GetTag() == tag) {
			count++;
		}
	}

	GameObjectContainer container(count);

	for (size_t i = 0; i < m_objects_count; i++) {
		if (m_objects[i]->GetTag() == tag) {
			container.Registar(m_objects[i]);
		}
	}

	return container;
}

GameObjectContainer GameWorld::GetGameObjects(const MyCircle& collision)
{
	size_t count{ 0 };

	for (size_t i = 0; i < m_objects_count; i++) {
		if (m_objects[i]->GetCollision().IsOverlap(collision)) {
			count++;
		}
	}

	GameObjectContainer container(count);

	for (size_t i = 0; i < m_objects_count; i++) {
		if (m_objects[i]->GetCollision().IsOverlap(collision)) {
			container.Registar(m_objects[i]);
		}
	}

	return container;
}

void GameWorld::Update()
{
	if (m_isPaused) {
		UpdateUI();
		return;	// ポーズ中は全て止めるよ^^
	}

	for (size_t i = 0; i < m_objects_count; i++) {
		m_objects[i]->Update();
	}

	//生死判定
	for (int i = ((int)m_objects_count) - 1; i >= 0; i--) {
		if (m_objects[i]->IsDestroy()) {
			delete m_objects[i];
			m_objects[i] = m_objects[--m_objects_count];
		}
	}
}

void GameWorld::UpdateUI()
{
	// すべてのオブジェクトのUI更新
	for (size_t i = 0; i < m_objects_count; i++) {
		m_objects[i]->UpdateUI();
	}
}

void GameWorld::Draw() const
{
	for (size_t i = 0; i < m_objects_count; i++) {
		m_objects[i]->Draw();
	}

}
