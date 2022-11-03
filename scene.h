#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "cameraBox2DtoSFML.h"
#include "priorityList.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <random>


namespace gamecore {

	namespace Events {
		enum
		{
			None = 0,
			MouseMoved = 1u << 0,
			MouseDown = 1u << 1,
		};
	}

	enum class ExitSceneCode
	{
		None = 0,
		CloseWindow,
		NewScene,
		Restart,
	};

	static const struct
	{
		const sf::Color base = { 255, 255, 255, 255 };
		const sf::Color highlighting = { 255, 191, 0, 255 };
		const sf::Color blocked = { 170, 170, 170, 255 };
	} ColorStyle;

	class EventHandler;
	class GUInterface;

	class Scene
	{
	public:
		Scene(sf::RenderWindow* window);

		virtual void MainLoop() = 0;
		virtual void Draw();
		virtual void MousePushCallBack(const int id) = 0;

		void AddTexture(const std::filesystem::path& file, const int texture, const bool isRepeated = false);
		void AddFont(const std::filesystem::path& file, const int font);

		sf::Texture* GetTexture(const int id) const;
		sf::Font* GetFont(const int id) const;

		/// <summary>
		/// Возвращает код с которым сцена завершена
		/// </summary>
		/// <returns></returns>
		ExitSceneCode GetExitCode() const;

		/// <summary>
		/// Создает графический элемент с указанными агрументами и добавляет его в очередь на отрисовку
		/// </summary>
		/// <typeparam name="T"> Тип графического элемента</typeparam>
		/// <typeparam name="...Args"> Набор аргументов</typeparam>
		/// <param name="priority"> - очередь на отрисовку. Чем ниже приоритет, тем раньше элемент будет отрисован</param>
		/// <param name="canBeDeleted"> - если элемент не будет удаляться тогда на него можно выделить меньше памяти</param>
		/// <returns></returns>
		template<typename T, typename ...Args>
		T* CreateGUInterface(const int priority, const bool canBeDeleted, Args&& ...);

		virtual ~Scene();
	private:
		/// <summary>
		/// Загружает из файла необходимые ресурсы и возвращает указатель на выделенный ресурс
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="file"></param>
		/// <returns></returns>
		template <typename T>
		T* LoadFromFile(const std::filesystem::path& file);

	protected:
		sf::RenderWindow* window_;
		std::unordered_map<int, std::unique_ptr<sf::Texture>> texturePool_;
		std::unordered_map<int, std::unique_ptr<sf::Font>> fontPool_;

		// Элементы управления
		PriorityList<sf::Drawable> userInterfaceDraw_;						// контейнер с указателями на элементы, которые необходимо отрисовывать

		Box2DCamera* camera_ = nullptr;
		EventHandler* eventsGame_ = nullptr;

		ExitSceneCode exitCode_ = ExitSceneCode::None;

	public:		// основные настройки
		const float WORLDSTEEP = 1 / 60.f;	// количество кадров в секунду для симуляции физического мира
		const float FRAMERATELIMIT = 60;	// 
		const int COLOR_COUNT = 5;			// количество цветов в коде
		const int COLOR_COUNT_MAX = 8;		// общее количество цветов в игре
		const bool COLOR_UNIQUE = true;		// должны ли цвета быть иникальными. Не уникальные цвета значительно усложняют процесс игры

	};

	template<typename T, typename ...Args>
	inline T* Scene::CreateGUInterface(const int priority, const bool canBeDeleted, Args&& ...arg)
	{
		T* ptr = new T(arg...);
		userInterfaceDraw_.Add(ptr, priority, canBeDeleted);
		return ptr;
	}

	template <typename T>
	T* Scene::LoadFromFile(const std::filesystem::path& file) {
		T* txt = new T;
		if (txt->loadFromFile(file))
		{
			txt->setSmooth(true);
			return txt;
		}

		assert(true, "Error load file: "s + file);
	}

	struct StepCore
	{
		b2World* world;								// текущий мир
		Box2DCamera* camera;						// текущая камера
		Scene* scene;								// текущее игровое ядро
		EventHandler* eventHandler;					// текущий обработчик событий

		size_t stepCount;							// количество шагов с момента запуска
		int prevDuration;							// время предыдущего шага, ms
		int curentDuration;							// время текущего шага, ms
	};

	int RandomGenerate(std::mt19937& generator, int min, int max);


} // namespace gamecore