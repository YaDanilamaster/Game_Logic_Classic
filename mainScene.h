#pragma once

#include "scene.h"

#include "queryBox2D.h"
#include "debugDraw.h"
#include "placeHolder.h"
#include "gameEvents.h"
#include "userInterface.h"

#include <unordered_set>
#include <utility>
#include <chrono>

namespace gamecore {

	using namespace std::string_literals;

	class EventHandler;
	class Sensor;
	class GUiFishka;


	class MainScene final : public Scene
	{
	public:

		enum class Texture
		{
			BackGround,
			Sprite,
			Fishka,
			Glass,
			PlaceHoldersLeft,
			PlaceHoldersCenter,
			PlaceHoldersRight,
			PlaceHoldersInfoLeft,
			PlaceHoldersInfoCenter,
			PlaceHoldersInfoRight,
			PlaceHoldersEmpty,
			PlaceHoldersHalf,
			PlaceHoldersFull,
			Border,
		};

		enum class Font
		{
			Message,
		};

		enum class Difficulty
		{
			Easy = 0,
			Normal,
			Hard,
		};
		const std::vector<std::wstring> difficultyToString = {L"легко", L"нормально", L"сложно"};

		enum class MenuItem
		{
			BackMenu = 0,
			NewGame,
			Check,
			Retreat,
		};

	public:
		MainScene(b2Vec2&& gravity, sf::RenderWindow* window);

		/// <summary>
		/// Инициализирует главный мир.
		/// </summary>
		void InitMainWorld(const int difficulty);

		void MainLoop() override;

		/// <summary>
		/// Создает тело фишки по указанным координатам
		/// </summary>
		b2Body* CreateBodyFishka(const float x, const float y);

		~MainScene();

	private:	// основные поля класса
		friend class ShowCode;
		friend class ShowMessage;

		int difficulty_;

		b2World* mainLevel_;
		StepCore stepCore_;
		Box2DCamera* camera_;
		sf::Vector2u defaultSize_;
		sf::Vector2f scale_;

		// поля для управления привязкой фишек к мышке
		b2Vec2 m_mouseWorld_;
		b2Body* m_groundBody_;
		b2MouseJoint* m_mouseJoint_ = NULL;

		// Класс необходимый для отрисовки отладочной информации из Box2D
		debugDraw::SFMLDebugDraw* debugDrawInstance_;

		// определяем поля для событий
		Sensor* sensor_;
		EventSystem<GUiLabel, const sf::Event::MouseMoveEvent&> eventsMouseMoved_;
		EventSystem<GUiLabel, const sf::Event::MouseButtonEvent&, Scene*> eventsMouseDown_;

		// Вспомогательные поля для ссылки на текстуры главного элемента игры
		PlaceHolder::PlaceHoldersTexture* mainPlaceTexture_;
		PlaceHolder::PlaceHoldersTexture* infoPlaceTexture_;
		PlaceHolder::PlaceHoldersInfoTexture* statePlaceTexture_;

		PlaceHolder* mainPlaceHolder_;

		GUiLabel* labelCheckPrt_;
		GUiLabel* labelRetreatPrt_;


		// Определяем места и количество для отображения предыдущих попыток
		std::vector<sf::Vector2f> tryPosition_{
			{950, 80},
			{950, 155},
			{950, 230},
			{950, 305},

			{950, 380},
			{950, 455},
			{950, 530},
			{950, 605},

			{950, 680},
			{950, 755},
			{950, 830},
			{950, 905},
		};

		size_t tryCount_;
		bool gameOver_ = false;

	private:
		// Создает непроходимые стены
		void CreateWall(const float xh, const float yh, const b2Vec2& center, float angle);

		void MouseDown(const b2Vec2& p);
		void MouseMove(const b2Vec2& p);
		void MouseUp(const b2Vec2& p);

		template<typename T, typename ...Args>
		T* CreateGUInterface(const int priority, const bool canBeDeleted, uint32_t events, Args&& ...);

		/// <summary>
		/// Формирует вектор с фишками нужного количества с Id в диапазоне от 0 до maxId
		/// </summary>
		/// <param name="count"> - количество фишек, которое нужно получить</param>
		/// <param name="maxId"> - максимальный id</param>
		/// <returns></returns>
		std::vector<GUiFishka*> MakeGUiFishka(const int count, const int maxId);

		/// <summary>
		/// Формирует вектор с фишками по входному вектору с их id
		/// </summary>
		/// <param name="items"> - вектор с id фишками</param>
		/// <returns> - вектор фишек с требуемыми id</returns>
		std::vector<GUiFishka*> MakeGUiFishka(const std::vector<int>& items);

		void LoadTexture();

		// Выводит на экран графические объекты
		void DrawGUI();

		/// <summary>
		/// Обрабатывает расставленные фишки
		/// </summary>
		/// <returns></returns>
		void ProcessHolders();

		// отключаем не нужные более элементы меню
		void BlockMenuItems();

		void MousePushCallBack(const int labelId) override;

		void FailedProcess();
		void TryInfo();
		void GameOver();
		void YouWin();

	};

	template<typename T, typename ...Args>
	inline T* MainScene::CreateGUInterface(const int priority, const bool canBeDeleted, uint32_t events, Args&& ...arg)
	{
		T* ptr = new T(arg...);
		userInterfaceDraw_.Add(ptr, priority, canBeDeleted);

		if (events & Events::MouseMoved)
		{
			eventsMouseMoved_.Subscribe(ptr, &GUiLabel::MouseHover);
		}
		if (events & Events::MouseDown)
		{
			eventsMouseDown_.Subscribe(ptr, &GUiLabel::MouseDown);
		}

		return ptr;
	}


	//
	// Вспомогательные функции и структуры
	//


	// Таблица текстур для файла с фишками
	static const std::vector<sf::IntRect> TextureFishkaByIndex{
		{{7, 7}, {77, 77}},		// 0
		{{93, 7}, {77, 77}},	// 1
		{{263, 7}, {77, 77}},	// 3

		{{7, 92}, {77, 77}},	// 5
		{{93, 92}, {77, 77}},	// 6
		{{263, 92}, {77, 77}},	// 8
		{{348, 92}, {77, 77}},	// 9

		{{93, 181}, {77, 77}},	// 11
		{{263, 181}, {77, 77}},	// 13
		{{348, 181}, {77, 77}},	// 14

		{{348, 7}, {77, 77}},	// 4
		{{178, 92}, {77, 77}},	// 7
		{{7, 181}, {77, 77}},	// 10
		{{178, 181}, {77, 77}},	// 12
		{{178, 7}, {77, 77}},	// 2

	};







} // namespace gamecore