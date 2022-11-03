#pragma once

#include "scene.h"

#include "coreEvents.h"
#include "userInterface.h"

#include <chrono>
#include <random>
#include <utility>


namespace gamecore {
	using namespace std::string_literals;

	class MenuScene final : public Scene
	{
	public:
		enum class Texture
		{
			BackGround,
			Sprite,
		};

		enum class Font
		{
			Message,
		};

		enum class MenuItem
		{
			NewGame		= 0,
			Settings,
			Rule,
			GoMenu,
			Easy,
			Normal,
			Hard,
		};

		enum class Difficulty
		{
			Easy		= 0,
			Normal,
			Hard,
		};

		MenuScene(sf::RenderWindow* window);

		void Initialization();
		void MainLoop() override;

		Difficulty GetDifficulty();

	private:

		void MousePushCallBack(const int labelId);

		template<typename T, typename ...Args>
		T* CreateGUInterface(const int priority, const bool canBeDeleted, uint32_t events, Args&& ...);


	private:
		EventSystem<GUiLabel, const sf::Event::MouseMoveEvent&> eventsMouseMoved_;
		EventSystem<GUiLabel, const sf::Event::MouseButtonEvent&, Scene*> eventsMouseDown_;

		sf::View ruleView_;
		sf::View settingsView_;


		sf::Vector2f centerMenu_;
		sf::Vector2f centerRule_{ 3000, 0 };
		sf::Vector2f centerSettings_{ 3000, 3000 };

		Difficulty difficulty_ = Difficulty::Normal;

		sf::String ruleText = sf::String(LR"( Компьютер загадывает цветовой код, цель игрока найти ключ к коду и взломать его!

                                               Базовая версия игры

  - Код состоит из 5 позицийю
  - Возможных цветов 8
  - Цвета в коде не повторяются.
  - На взлом дается 12 попыток

 На каждом ходе компьютер дает подсказку в следующем формате:
 Полностью закрашенный кружок - полное совпадение цвета и позиции.
 Половина кружка закрашена - совпадение по цвету но он на неправильной позиции.
 Пустой кружок - цвет отсутствует.

 Важно! Положение подсказок не соответствует положению цветов. Например, если в
 подсказке указан один полностью закрашенный кружок и четыре полукружка - значит
 в коде совпали все цвета, но какой то один цвет стоит на своем месте, а остальные
 на неверных позициях. Какие именно - это и есть ЛОГИКА!

 Неоценимую помощь Вам окажет информация по предыдущим попыткам.
)");


	};

	template<typename T, typename ...Args>
	inline T* MenuScene::CreateGUInterface(const int priority, const bool canBeDeleted, uint32_t events, Args&& ...arg)
	{
		T* ptr = new T(arg...);
		userInterfaceDraw_.Add(ptr, priority, canBeDeleted);

		if (events & Events::MouseMoved)
		{
			eventsMouseMoved_.Subscribe(ptr, &T::MouseHover);
		}
		if (events & Events::MouseDown)
		{
			eventsMouseDown_.Subscribe(ptr, &T::MouseDown);
		}

		return ptr;
	}


} // namespace gamecore