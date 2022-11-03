#pragma once

#include "SFML/Graphics.hpp"
#include "box2d/box2d.h"

#include "cameraBox2DtoSFML.h"
#include "logic.h"
#include "userInterface.h"

#include <iostream>
#include <vector>


namespace gamecore {

	enum class ProcessHoldersResult
	{
		Failed,
		Hold,
		Hacked,
	};

	/// <summary>
	/// Представляет контейнер для хранения и управления фишками, а также отображения информации о результатах их сравнения.
	/// </summary>
	class PlaceHolder final : public sf::Drawable
	{
	public:
		using VectorSprites = std::vector<sf::Sprite*>;

		//Текстура, описывающая необходимые поля для отображения одного элемента PlaceHolder
		struct PlaceHoldersTexture
		{
			sf::Texture* leftBorder;
			sf::Texture* centerBorder;
			sf::Texture* rightBorder;
		};
		struct PlaceHoldersInfoTexture
		{
			sf::Texture* empty;
			sf::Texture* half;
			sf::Texture* full;
		};


	public:
		PlaceHolder(
			PlaceHoldersTexture& mainHolder,
			PlaceHoldersTexture& infoHolder,
			PlaceHoldersInfoTexture& indicators,
			size_t placeCount
		);

		// Создает копию только графической части объекта, без сенсоров и цветового кода.
		// Используется для создания индикаторов
		PlaceHolder(const PlaceHolder& other);
		// Объект надлежит создать только через фабрику GameCore
		PlaceHolder& operator=(PlaceHolder&) = delete;

		/// <summary>
		/// Устанавливает начальную позицию элемента. Отсчет ведется от верхнего левого угла
		/// </summary>
		/// <param name="pos"> - координаты начальной позиции элемента</param>
		void SetPosition(const sf::Vector2f& pos);

		/// <summary>
		/// Применяет указанные коэффициенты к текущему масштабу
		/// </summary>
		/// <param name="scale"> - коэффициент масштабирования</param>
		void SetScale(const sf::Vector2f& scale);

		/// <summary>
		/// Выравнивает информационную панель по центру главной панели
		/// </summary>
		void SetInfoHolderAlignCenter();

		/// <summary>
		/// Создает физическое тело элемента со всеми необходимыми сенсорами, связями и держателями.
		/// </summary>
		/// <param name="world"> - мир в котором необходимо создать элементы</param>
		/// <param name="camera"> - ссылка на камеру для корректной связки графических координат с физическими</param>
		/// <param name="window"> - ссылка на окно для корректной связки графических координат с физическими</param>
		void CreateHoldersBody(b2World* world, Box2DCamera* camera, sf::RenderWindow* window);

		/// <summary>
		/// Создает новый цветовой код. Коды генирируются в диаппазоне от 0 до maxColorCount
		/// </summary>
		/// <param name="maxColorCount"> - максимальное количество цветов</param>
		/// <param name="uniqueColor"> - должен ли цвет быть уникальным</param>
		void CreateNewCode(const int maxColorCount, const bool uniqueColor = true);

		/// <summary>
		/// Обрабатывает захваченные сенсорами фишки и выводит результат обработки цветовога кода на информационную панель элемента.
		/// </summary>
		/// <returns> - возвращаяет результат взлома кода</returns>
		/// <param name="matching_positions"> - должны ли позиции индикатора совпадать с позицией кода</param>
		ProcessHoldersResult ProcessHolders(const bool matching_positions);

		/// <summary>
		/// Получает исходный цветовой код
		/// </summary>
		const std::vector<int>& GetCode() const;

		virtual ~PlaceHolder() override;

	private:
		PlaceHoldersTexture& mainHolder_;
		PlaceHoldersTexture& infoHolder_;
		PlaceHoldersInfoTexture& indicators_;

		// спрайты для отрисовки элементов контейнера
		VectorSprites mainHolderSprite_;
		VectorSprites infoHolderSprite_;
		VectorSprites indicatorsSprite_;
		VectorSprites itemsSprite_;

		// Массив с исходными состояниями индикатора
		VectorSprites indicatorsItem_;
		std::vector<Compliance> compilance_;

		// Смещение центра отностительно центра текстуры
		sf::Vector2f offsetMainCenter_;
		sf::Vector2f offsetInfoCenter_;

		b2World* world_;
		std::vector<b2Body*> b2HolderPosition_;						// массив с указателями на сенсоры в элементе
		std::vector<B2BodyUserDataD*> b2UserDataPtr_;
		
		Box2DCamera* camera_;
		sf::RenderWindow* window_;

		sf::Vector2f pos_;
		sf::FloatRect mainRect_;
		sf::FloatRect infoRect_;

		Logic* logic = nullptr;

		size_t placeCount_;
		size_t placeSprites_;

		bool allComplite_;

	private:
		void CreatePlaces(
			PlaceHoldersTexture& holderTexture,
			VectorSprites& places,
			const sf::Vector2f& scale,
			const sf::Color& color
		);

		void Construct();
		void CreateIndicators(const sf::Vector2f& scale, const sf::Color& color);

		sf::FloatRect SetHolderPosition(VectorSprites& places, const sf::Vector2f& pos);
		void SetStatePosition(const sf::Vector2f& pos);
		void SetSensorPosition(const sf::Vector2f& pos);
		void SetItemsPosition();
		void SetHoldersScale(VectorSprites& places, const sf::Vector2f& scale);

		// Устанавливает указанный индикатор в состояние state
		void SetInfoStatus(const size_t id, const Compliance state);

		// Создает в физическом мире сенсорные объекты для управления физикой фишек
		b2Body* CreateBody(const float xh, const float yh, const b2Vec2& center);

		virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	};



} // namespace gamecore