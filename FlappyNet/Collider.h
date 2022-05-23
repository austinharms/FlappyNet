#pragma once
#include <SFML/System.hpp>

class Collider
{
public:
	virtual sf::Vector2f getPosition() const = 0;
	virtual sf::Vector2f getSize() const = 0;
};