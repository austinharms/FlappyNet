#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include <ctime>
#include "Net.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Flappy Net");
	window.setKeyRepeatEnabled(false);

	Game game;
	Player p;
	Net n(3, 10, 5, 1);

	game.Start();
	float dt = 0;
	clock_t last = clock();
	while (window.isOpen())
	{
		clock_t now = clock();
		dt = ((float)(now - last) / CLOCKS_PER_SEC) * 100;

		last = now;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				p.Jump();
			}
		}

		window.clear(sf::Color::Black);
		if (game.CheckHit(p)) {
			p.SetColor(sf::Color(255, 0, 0));
			game.Reset();
		}
		else
		{
			p.SetColor(sf::Color(0, 255, 0));
		}

		p.Update(window, dt);
		game.Update(window, dt);
		window.display();
	}

	return 0;
}