#ifndef COLLISION_H
#define COLLISION_H


class MyCircle;
class MyPoint;

class Collision
{
public:
	virtual ~Collision() = default;

	virtual bool IsOverlap(const MyCircle& circle) const { return false; }
	virtual bool IsOverlap(const MyPoint& circle) const { return false; }


};


class MyCircle : public Collision
{
private:
	Vec2 m_center{};
	float m_radius{};

public:
	MyCircle(const Vec2& center, float radius) : m_center(center), m_radius(radius){}
	const Vec2& GetCenter() const { return m_center; }

	bool IsOverlap(const MyCircle& target) const override{
		return m_center.distanceFromSq(target.m_center) <= (m_radius + target.m_radius)* (m_radius + target.m_radius);
	}

	bool IsOverlap(const MyPoint& target) const override;
	

};

class MyPoint : public Collision
{
private:
	Vec2 m_center{};
public:
	MyPoint(const Vec2& center) : m_center(center){}
	const Vec2& GetCenter() const { return m_center; }

	bool IsOverlap(const MyCircle& target) const override {
		return target.IsOverlap(*this);

	}


};













#endif // COLLISION_H
