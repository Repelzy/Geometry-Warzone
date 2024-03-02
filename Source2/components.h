#pragma once
#include "vec2.h"
#include <SFML/Graphics.hpp>

class CTransform
{
public:
	Vec2 pos = {0.0,0.0 };
	Vec2 velocity = { 0.0,0.0 };
	float angle = 0;

	CTransform(const Vec2& p, const Vec2& s, float a)
		: pos(p),
		velocity(s),
		angle(a) {}
};


class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness)
		: circle(radius, points)
	{
		circle.setOutlineColor(outline);
		circle.setFillColor(fill);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}

};

class CCollision
{
public:
	float radius = 0;

	CCollision(float r)
		: radius(r) {}
};

class CScore
{
public:
	int score;
	CScore(int s)
		: score(s) {}
};

class CLifeSpan
{
public:
	int remaining = 0 ;		
	int total = 0 ;				//(ie.substract 1 every frame for int remaining )
								//Amount of lifespan remaining on the entity
	CLifeSpan(int total)		//Initial amount of lifespan
		: remaining(total), total(total) {}
};

class CInput
{
public:
	bool up = false ;
	bool down = false;
	bool left = false ;
	bool right = false ;
	bool leftMouse = false ;
	bool rightMouse = false ;
	CInput() {}
};