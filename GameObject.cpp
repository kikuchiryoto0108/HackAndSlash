#include "stdafx.h"
#include "GameObject.h"
#include "GameWorld.h"

GameObject::GameObject(GameWorld* owner, const std::string& tag)
	: m_owner(owner), m_tag(tag)
{
}
