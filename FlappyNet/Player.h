#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Player
{
public:
	const float PLAYER_SIZE = 30;
	const float PLAYER_GRAVITY = 0.003;
	const float PLAYER_JUMP_STRENGTH = 5;
	Player() : _player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE)) {
		_player.setPosition(200, 300);
		_acceleration = 0;
	}

	void Update(sf::RenderWindow& window, float dt) {
		_acceleration += PLAYER_GRAVITY;
		_player.move(sf::Vector2f(0, _acceleration * dt));
		window.draw(_player);
	}

	void Jump() {
		_acceleration = -PLAYER_JUMP_STRENGTH;
	}

	void SetColor(sf::Color c) {
		_player.setFillColor(c);
	}

	sf::Vector2f GetPos() {
		return _player.getPosition();
	}

private:
	sf::RectangleShape _player;
	float _acceleration;
};