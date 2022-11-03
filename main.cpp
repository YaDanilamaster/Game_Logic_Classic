#include "SFML/Graphics.hpp"

#include <memory>
#include <iostream>

#include "scene.h"
#include "menuScene.h"
#include "mainScene.h"

using namespace std::string_literals;

int main()
{
	// настраиваем окно для рендеринга
	std::unique_ptr<sf::RenderWindow> window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ 1280, 900 }), "Logic Classic"s, sf::Style::Default);
	window->setFramerateLimit(60);
	window->setVerticalSyncEnabled(true);

	int difficult = 1;

	gamecore::ExitSceneCode exitCode = gamecore::ExitSceneCode::NewScene;

	while (exitCode != gamecore::ExitSceneCode::CloseWindow) {

		if (exitCode == gamecore::ExitSceneCode::NewScene) {
			// запускаем сцену с меню
			{
				std::unique_ptr<gamecore::MenuScene> menuScena = std::make_unique<gamecore::MenuScene>(window.get());

				menuScena->Initialization();
				menuScena->MainLoop();

				if (menuScena->GetExitCode() == gamecore::ExitSceneCode::CloseWindow)
				{
					return 0;
				}

				difficult = (int)menuScena->GetDifficulty();
			}
		}

		// запускаем главную сцену
		{
			std::unique_ptr<gamecore::MainScene> mainScene = std::make_unique<gamecore::MainScene>(b2Vec2{ 0.f, 9.8f }, window.get());

			mainScene->InitMainWorld(difficult);
			mainScene->MainLoop();

			exitCode = mainScene->GetExitCode();
		}
	}

	return 0;
}
