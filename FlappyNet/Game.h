#pragma once
#include <SFML/Graphics.hpp>
#include "Pipe.h"
#include <stdint.h>
#include "Collider.h"

class Game
{
public:
	const float START_SPEED = 150;
	const float ACCELERATION = 3;
	const float PIPE_DISTANCE = 300;
	static const uint8_t PIPE_COUNT = 5;
	Game() {
		_running = false;
		reset();
	}

	void start() {
		_running = true;
	}

	void reset() {
		_speed = START_SPEED;
		for (uint8_t i = 0; i < PIPE_COUNT; ++i) {
			//_pipes[i].setPosition(200 + (PIPE_DISTANCE * (i + 1)), rand() % 400 + 100);
			_pipes[i].setPosition(200 + (PIPE_DISTANCE * (i + 1)), rand() % 200 + 200);
		}

		_pipes[0].setPosition(200 + PIPE_DISTANCE, 350);
		_nextPipe = _pipes;
	}

	void stop() {
		_running = false;
	}

	void update(float dt) {
		if (!_running) return;
		_speed += ACCELERATION * dt;
		for (uint8_t i = 0; i < PIPE_COUNT; ++i) {
			_pipes[i].move(_speed * dt);
			if (_pipes[i].getPos() < 200 + PIPE_DISTANCE - Pipe::PIPE_WIDTH && _pipes[i].getPos() + Pipe::PIPE_WIDTH > 200)
				_nextPipe = _pipes + i;

			if (_pipes[i].getPos() <= 0) {
				_pipes[i].setPosition(PIPE_COUNT * PIPE_DISTANCE, rand() % 200 + 200);
				//_pipes[i].setPosition(PIPE_COUNT * PIPE_DISTANCE, 350);
			}
		}
	}

	void draw(sf::RenderWindow& window) {
		for (uint8_t i = 0; i < PIPE_COUNT; ++i) {
			if (_nextPipe == _pipes + i) {
				_pipes[i].setColor(sf::Color(0, 0, 255));
			}
			else {
				_pipes[i].setColor(sf::Color(255, 255, 255));
			}
			_pipes[i].draw(window);
		}
	}

	bool checkHitPipe(const Collider& c) const {
		return checkOverlap(c.getPosition(), c.getSize(), _nextPipe->getTopPos(), sf::Vector2f(_nextPipe->PIPE_WIDTH, _nextPipe->PIPE_HEIGHT)) || checkOverlap(c.getPosition(), c.getSize(), _nextPipe->getBottomPos(), sf::Vector2f(_nextPipe->PIPE_WIDTH, _nextPipe->PIPE_HEIGHT));
	}

	bool checkOverlap(const sf::Vector2f pos1, const sf::Vector2f size1, const sf::Vector2f pos2, const sf::Vector2f size2) const {
		return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x && pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
	}

	Pipe* getNextPipe() const {
		return _nextPipe;
	}

	float getSpeed() const {
		return _speed/START_SPEED;
	}

private:
	Pipe _pipes[PIPE_COUNT];
	Pipe* _nextPipe;
	bool _running;
	float _speed = START_SPEED;
};