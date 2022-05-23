#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <thread>
#include <fstream>
#include <malloc.h>
#include "Game.h"
#include "NetPlayer.h"
#include "Player.h"
#include "Net/Net.h"
#include "ctpl_stl.h"

const float FIXED_STEP = 0.01f;
const float AI_FIXED_STEP = 0.05f;

void updateAIPlayer(int threadId, NetPlayer* player, float timeStep) {
	player->calculate(timeStep);
	player->update(timeStep);
}

void trainAIPlayers(sf::RenderWindow& window, uint32_t playerCount, const char* savePath = nullptr) {
	float bestTime = 0;
	uint8_t* bestNet = nullptr;
	uint64_t bestNetSize = 0;
	uint8_t* lastNet = nullptr;
	uint64_t lastNetSize = 0;
	uint64_t generationCount = 0;
	uint64_t lastSavedGeneration = 0;

	// Cretae Thread Pool
	uint32_t poolSize = std::thread::hardware_concurrency() - 1;
	ctpl::thread_pool pool(poolSize);

	// Create Game Instance
	Game game;

	// Create AI Players
	NetPlayer** netPlayers = (NetPlayer**)malloc(playerCount * sizeof(NetPlayer*));
	{
		Net* defaultNet;
		if (savePath == nullptr) {
			defaultNet = new Net(7, 1, 7, 15);
		}
		else {
			std::ifstream netFile(savePath, std::ios::binary);
			netFile.seekg(0, std::ios::end);
			uint64_t fileSize = netFile.tellg();
			netFile.seekg(0, std::ios::beg);
			uint8_t* fileBytes = (uint8_t*)malloc(fileSize);
			netFile.read((char*)fileBytes, fileSize);
			defaultNet = new Net(fileBytes);
		}

		for (uint32_t i = 0; i < playerCount; ++i) {
			netPlayers[i] = new NetPlayer(&game, *defaultNet);
			if (i != 0)
				netPlayers[i]->getNet()->randomise();
		}

		delete defaultNet;
	}

	game.start();
	// Create Game Loop
	while (window.isOpen())
	{
		// Run 20 Steps Per Frame
		for (uint8_t c = 0; c < 20; ++c)
		{
			// Update AI Players on Thread Pool
			for (int i = 0; i < playerCount; ++i)
				pool.push(updateAIPlayer, netPlayers[i], AI_FIXED_STEP);

			// Wait for AI Updates
			while (pool.n_idle() != poolSize);

			// Update Game/Pipes
			game.update(AI_FIXED_STEP);

			// Update AI Player Net if Game Over
			bool playersAlive = false;
			for (uint32_t i = 0; i < playerCount; ++i) {
				if (netPlayers[i]->isAlive()) {
					playersAlive = true;
					break;
				}
			}

			if (!playersAlive) {
				NetPlayer* bestPlayer = netPlayers[0];
				for (uint32_t i = 1; i < playerCount; ++i) {
					if (netPlayers[i]->getLifetime() >= bestPlayer->getLifetime())
						bestPlayer = netPlayers[i];
				}

				std::cout << "Best: " << bestPlayer->getLifetime() << std::endl;
				free(lastNet);
				lastNet = bestPlayer->getNet()->getBytes();
				lastNetSize = bestPlayer->getNet()->getSize();
				if (bestPlayer->getLifetime() > bestTime) {
					bestTime = bestPlayer->getLifetime();
					free(bestNet);
					bestNet = bestPlayer->getNet()->getBytes();
					bestNetSize = bestPlayer->getNet()->getSize();
				}

				for (uint32_t i = 0; i < playerCount; ++i) {
					if (netPlayers[i] != bestPlayer) {
						delete netPlayers[i];
						netPlayers[i] = new NetPlayer(&game, *bestPlayer->getNet());
						netPlayers[i]->getNet()->randomise();
					}
					else {
						netPlayers[i] = new NetPlayer(&game, *bestPlayer->getNet());
					}
				}

				delete bestPlayer;
				game.reset();
				++generationCount;
			}
		}

		// Clear Frame
		window.clear(sf::Color::Black);

		// Draw AI Players
		for (int i = 0; i < playerCount; ++i)
			netPlayers[i]->draw(window);

		// Draw Game/Pipes
		game.draw(window);

		// Show the frame
		window.display();

		// Save Best Net Every 10 Generations
		if (generationCount % 10 == 0 && lastSavedGeneration != generationCount) {
			lastSavedGeneration = generationCount;
			std::ofstream best("best.net", std::ios::binary);
			best.write((const char*)bestNet, bestNetSize);
			best.close();

			std::ofstream last("last.net", std::ios::binary);
			last.write((const char*)lastNet, lastNetSize);
			last.close();
			std::cout << "Saved Gen: " << generationCount << std::endl;
		}

		// Check Window Events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
				lastSavedGeneration = generationCount;
				std::ofstream best("best.net");
				best.write((const char*)bestNet, bestNetSize);
				best.close();

				std::ofstream last("last.net");
				last.write((const char*)lastNet, lastNetSize);
				last.close();
				std::cout << "Saved" << std::endl;
			}
		}
	}
}

void runAIPlayer(sf::RenderWindow& window, const char* savePath) {
	// Create Game Instance
	Game game;

	// Load NetSave
	std::ifstream netFile(savePath, std::ios::binary);
	netFile.seekg(0, std::ios::end);
	uint64_t fileSize = netFile.tellg();
	netFile.seekg(0, std::ios::beg);
	uint8_t* fileBytes = (uint8_t*)malloc(fileSize);
	netFile.read((char*)fileBytes, fileSize);
	Net playerNet(fileBytes);
	free(fileBytes);

	// Create AI Player
	NetPlayer* netPlayer = new NetPlayer(&game, playerNet);

	game.start();

	// Create Update Timers
	float accumulator = 0;
	float aiAccumulator = 0;
	clock_t last = clock();
	clock_t now;

	// Create Game Loop
	while (window.isOpen())
	{
		// Update Update Timer
		now = clock();
		accumulator += ((float)(now - last) / CLOCKS_PER_SEC);
		last = now;

		while (accumulator >= FIXED_STEP)
		{
			accumulator -= FIXED_STEP;
			aiAccumulator += FIXED_STEP;
			if (aiAccumulator >= AI_FIXED_STEP) {
				aiAccumulator -= AI_FIXED_STEP;

				// Update AI Player
				netPlayer->calculate(AI_FIXED_STEP);
				if (!netPlayer->isAlive()) {
					delete netPlayer;
					netPlayer = new NetPlayer(&game, playerNet);
					game.reset();
				}
			}

			netPlayer->update(FIXED_STEP);

			// Update Game/Pipes
			game.update(FIXED_STEP);
		}

		// Draw Frame
		window.clear(sf::Color::Black);
		netPlayer->draw(window);
		game.draw(window);
		window.display();

		// Check Window Events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
	}

	delete netPlayer;
}

void playPlayer(sf::RenderWindow& window) {
	// Create Game Instance
	Game game;

	// Create Player
	Player* player = new Player(&game);

	// Create Update Timers
	float accumulator = 0;
	clock_t last = clock();
	clock_t now;

	game.start();
	// Create Game Loop
	while (window.isOpen())
	{
		// Update Update Timer
		now = clock();
		accumulator += ((float)(now - last) / CLOCKS_PER_SEC);
		last = now;

		while (accumulator >= FIXED_STEP)
		{
			accumulator -= FIXED_STEP;
			// Update Player
			player->update(FIXED_STEP);
			if (!player->isAlive()) {
				std::cout << "Time: " << player->getLifetime() << std::endl;
				delete player;
				player = new Player(&game);
				game.reset();
			}

			// Update Game/Pipes
			game.update(FIXED_STEP);
		}

		// Draw Frame
		window.clear(sf::Color::Black);
		player->draw(window);
		game.draw(window);
		window.display();

		// Check Window Events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
				player->jump();
			}
		}
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Flappy Net");
	window.setKeyRepeatEnabled(false);
	//playPlayer(window);
	trainAIPlayers(window, 3000);
	//runAIPlayer(window, "best.net");
	return 0;
}