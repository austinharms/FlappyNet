#pragma once
#include <SFML/Graphics.hpp>
#include "Pipe.h"
#include <stdint.h>
#include "Player.h"

class Game
{
public:
	const float START_SPEED = 1;
	const float ACCELERATION = 0.001f;
	const float PIPE_DISTANCE = 300;
	static const uint8_t PIPE_COUNT = 5;
	Game() {
		_running = false;
		Reset();
	}

	void Start() {
		_running = true;
	}

	void Reset() {
		_speed = START_SPEED;
		for (uint8_t i = 0; i < PIPE_COUNT; ++i) {
			_pipes[i].SetPosition(750 + (PIPE_DISTANCE * i), rand()%400 + 100);
		}
	}

	void Stop() {
		_running = false;
	}

	void Update(sf::RenderWindow& window, float dt) {
		if (!_running) return;
		_speed += ACCELERATION * dt;
		for (uint8_t i = 0; i < PIPE_COUNT; ++i) {
			_pipes[i].Move(_speed * dt);
			if (_pipes[i].GetPos() <= 0) {
				_pipes[i].SetPosition(PIPE_COUNT * PIPE_DISTANCE, rand() % 400 + 100);
			}

			_pipes[i].Update(window, dt);
		}
	}

	bool CheckHit(Player& player) {
		for (uint8_t i = 0; i < PIPE_COUNT; ++i) {
			if (CheckOverlap(player.GetPos(), sf::Vector2f(player.PLAYER_SIZE, player.PLAYER_SIZE), _pipes[i].GetTopPos(), sf::Vector2f(_pipes[i].PIPE_WIDTH, _pipes[i].PIPE_HEIGHT))) return true;
			if (CheckOverlap(player.GetPos(), sf::Vector2f(player.PLAYER_SIZE, player.PLAYER_SIZE), _pipes[i].GetBottomPos(), sf::Vector2f(_pipes[i].PIPE_WIDTH, _pipes[i].PIPE_HEIGHT))) return true;
		}

		return false;
	}

	bool CheckOverlap(sf::Vector2f pos1, sf::Vector2f size1, sf::Vector2f pos2, sf::Vector2f size2)
	{
		return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x && pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
	}

private:
	Pipe _pipes[PIPE_COUNT];
	bool _running;
	float _speed = START_SPEED;
};