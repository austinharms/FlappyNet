#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Pipe
{
public:
	const static float PIPE_WIDTH;
	const static float PIPE_HEIGHT;
	const static float PIPE_GAP;
	Pipe() : _top(sf::Vector2f(PIPE_WIDTH, PIPE_HEIGHT)), _bottom(sf::Vector2f(PIPE_WIDTH, PIPE_HEIGHT)) {
		setPosition(0, 0);
	}

	void move(float amount) {
		_top.move(sf::Vector2f(-amount, 0));
		_bottom.move(sf::Vector2f(-amount, 0));
	}

	void setPosition(float x, float height) {
		_top.setPosition(sf::Vector2f(x, -PIPE_HEIGHT - PIPE_GAP + height));
		_bottom.setPosition(sf::Vector2f(x, height));
	}

	float getPos() const {
		return _top.getPosition().x;
	}

	void setColor(sf::Color c) {
		_top.setFillColor(c);
		_bottom.setFillColor(c);
	}

	void draw(sf::RenderWindow& window) {
		window.draw(_top);
		window.draw(_bottom);
	}

	float getTopY() const {
		return _top.getPosition().y + PIPE_HEIGHT;
	}

	float getBottomY() const {
		return _bottom.getPosition().y;
	}

	sf::Vector2f getTopPos() const {
		return _top.getPosition();
	}

	sf::Vector2f getBottomPos() const {
		return _bottom.getPosition();
	}

private:
	sf::RectangleShape _top;
	sf::RectangleShape _bottom;
};

const float Pipe::PIPE_WIDTH = 50;
const float Pipe::PIPE_HEIGHT = 800;
const float Pipe::PIPE_GAP = 200;