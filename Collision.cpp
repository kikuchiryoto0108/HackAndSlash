#include "stdafx.h"
#include "Collision.h"

bool MyCircle::IsOverlap(const MyPoint& target) const
{
	return m_center.distanceFromSq(target.GetCenter()) <= m_radius * m_radius;
}
