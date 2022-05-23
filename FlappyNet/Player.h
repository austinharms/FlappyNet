#pragma once
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Collider.h"
#include "Game.h"

class Player : public Collider
{
public:
	const static float PLAYER_SIZE;
	const static float PLAYER_SPAWN_X;
	const static float PLAYER_SPAWN_Y;
	const static float PLAYER_GRAVITY;
	const static float PLAYER_JUMP_STRENGTH;
	Player(Game* game) : _player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE)) {
		_game = game;
		_player.setPosition(PLAYER_SPAWN_X, PLAYER_SPAWN_Y);
		_acceleration = 0;
		_live = true;
		_lifetime = 0;
	}

	void update(float dt) {
		if (_live) {
			if (_game->checkHitPipe(*this) || _player.getPosition().y > 600 - PLAYER_SIZE || _player.getPosition().y < 0) {
				_live = false;
				return;
			}

			_lifetime += dt;
			_acceleration += PLAYER_GRAVITY * dt;
			_player.move(sf::Vector2f(0, _acceleration * dt));
		}
	}

	void draw(sf::RenderWindow& window) {
		window.draw(_player);
	}

	void jump() {
		_acceleration = -PLAYER_JUMP_STRENGTH;
	}

	sf::Vector2f getPosition() const {
		return _player.getPosition();
	}

	sf::Vector2f getSize() const {
		return sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE);
	}

	bool isAlive() const {
		return _live;
	}

	float getLifetime() const {
		return _lifetime;
	}

protected:
	Game* _game;
	sf::RectangleShape _player;
	float _acceleration;
	bool _live;
	float _lifetime;
};

const float Player::PLAYER_SIZE = 30;
const float Player::PLAYER_SPAWN_X = 200;
const float Player::PLAYER_SPAWN_Y = 300;
const float Player::PLAYER_GRAVITY = 500;
const float Player::PLAYER_JUMP_STRENGTH = 300;
