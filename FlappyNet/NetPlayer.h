#pragma once
#include <ctime>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <random>
#include "Net/Net.h"
#include "Game.h"
#include "Collider.h"
#include "Pipe.h"

class NetPlayer : public Collider
{
public:
	const static float PLAYER_SIZE;
	const static float PLAYER_SPAWN_X;
	const static float PLAYER_SPAWN_Y;
	const static float PLAYER_GRAVITY;
	const static float PLAYER_JUMP_STRENGTH;
	NetPlayer(Game* game, const Net& net) : _net(net), _player(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE)) {
		_jumpedLast = 0;
		_lifetime = 0;
		_game = game;
		_player.setPosition(PLAYER_SPAWN_X, PLAYER_SPAWN_Y);
		_acceleration = 0;
		_live = true;
		_player.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255));
	}

	void calculate(float dt) {
		if (_live) {
			if (_game->checkHitPipe(*this) || _player.getPosition().y > 600 - PLAYER_SIZE || _player.getPosition().y < 0) {
				_live = false;
				return;
			}

			_net.setInput(0, _jumpedLast);
			_net.setInput(1, _game->getSpeed());
			Pipe* next = _game->getNextPipe();
			_net.setInput(2, (next->getPos() - 200) / 600);
			_net.setInput(3, (next->getTopY() - _player.getPosition().y) / 600);
			_net.setInput(4, (_player.getPosition().y - (next->getBottomY() + PLAYER_SIZE)) / 600);
			_net.setInput(5, _player.getPosition().y / 600);
			_net.setInput(6, _acceleration);
			_net.compute();
			if (_net.getOutput(0) >= 1) {
				_jumpedLast = 1;
				_acceleration = -PLAYER_JUMP_STRENGTH;
			}
			else {
				_jumpedLast = 0;
			}
		}
	}

	void update(float dt) {
		if (_live) {
			_lifetime += dt;
			_acceleration += PLAYER_GRAVITY * dt;
			_player.move(sf::Vector2f(0, _acceleration * dt));
		}
	}

	void draw(sf::RenderWindow& window) {
		if (_live)
			window.draw(_player);
	}

	Net* getNet() const {
		return (Net*)&_net;
	}

	float getLifetime() const {
		return _lifetime;
	}

	bool isAlive() const {
		return _live;
	}

	sf::Vector2f getPosition() const {
		return _player.getPosition();
	}

	sf::Vector2f getSize() const {
		return sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE);
	}

private:
	Game* _game;
	sf::RectangleShape _player;
	Net _net;
	float _jumpedLast;
	float _acceleration;
	float _lifetime;
	bool _live;
};

const float NetPlayer::PLAYER_SIZE = 30;
const float NetPlayer::PLAYER_SPAWN_X = 200;
const float NetPlayer::PLAYER_SPAWN_Y = 300;
const float NetPlayer::PLAYER_GRAVITY = 500;
const float NetPlayer::PLAYER_JUMP_STRENGTH = 300;