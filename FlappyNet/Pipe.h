#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Pipe
{
public:
	const float PIPE_WIDTH = 50;
	const float PIPE_HEIGHT = 800;
	const float PIPE_GAP = 200;
	Pipe() : _top(sf::Vector2f(PIPE_WIDTH, PIPE_HEIGHT)), _bottom(sf::Vector2f(PIPE_WIDTH, PIPE_HEIGHT)) {
		SetPosition(0, 0);
	}

	void Move(float amount) {
		_top.move(sf::Vector2f(-amount, 0));
		_bottom.move(sf::Vector2f(-amount, 0));
	}

	void SetPosition(float x, float height) {
		_top.setPosition(sf::Vector2f(x, -PIPE_HEIGHT - PIPE_GAP + height));
		_bottom.setPosition(sf::Vector2f(x, height));
	}

	float GetPos() {
		return _top.getPosition().x;
	}

	void Update(sf::RenderWindow& window, float dt) {
		window.draw(_top);
		window.draw(_bottom);
	}

	sf::Vector2f GetTopPos() {
		return _top.getPosition();
	}

	sf::Vector2f GetBottomPos() {
		return _bottom.getPosition();
	}

private:
	sf::RectangleShape _top;
	sf::RectangleShape _bottom;
};