#pragma once

#include "SFML/Graphics.hpp"
#include "box2d/box2d.h"

#include "cameraBox2DtoSFML.h"
#include "coreEvents.h"
#include "scene.h"

#include <iostream>

/*
					Drawable
						|
				  --GUInterface  --------------------------
				/			|	 			|				\
			GUiButton	GUiFishka		GUiSprite		  GUiText
		   /		\										|
GUiButtonChekc	GUiButtonNew							GUiLabel

*/


namespace gamecore {

	class Scene;
	class GUInterface;

	// Типы пользовательских графических элементов
	enum class UiType
	{
		Fishka = 0,
		Button,
		PlaceHolder,
		Sensor,
		Joint,
		Sprite,
		Text,
		Label,
	};

	// Вспомогательная структура для хранения пользовательской информации в объектах box2d
	struct B2BodyUserDataD
	{
		UiType itemType;
		GUInterface* itemPointer;
		int cash;
		bool isDrawable;
	};

	/// <summary>
	/// Базовый класс для всех графических элементов игры
	/// </summary>
	class GUInterface : public sf::Drawable
	{
	public:
		GUInterface(const UiType guiType);
		GUInterface(sf::Texture* texture, const UiType guiType);
		GUInterface(sf::Texture* texture, sf::IntRect rect, const int id, const UiType guiType);
		const UiType type;
		virtual void SetPosition(const sf::Vector2f& pos);
		virtual void SetRotation(const sf::Angle& angle);

		virtual const sf::IntRect& GetRect() const;
		virtual sf::Sprite& GetSprite();
		virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;
		virtual ~GUInterface() = default;
	protected:
		sf::Sprite sprite_;
		sf::Texture* texture_;
		sf::IntRect rect_;
		int id_ = -1;

	protected:
		const bool Contains(const int x, const int y) const;

	};

	/// <summary>
	/// Представляет родительский класс для всех графических интерфейсоф - кнопка
	/// </summary>
	class GUiButton : public GUInterface
	{
	public:
		GUiButton(sf::Texture* texture, const sf::IntRect rect, const int id);

		virtual void MouseDown(const sf::Event::MouseButtonEvent& downPos, Scene* sc) = 0;
		virtual void MouseHover(const sf::Event::MouseMoveEvent& movPos) = 0;

	protected:
		sf::Color defaultColor_;
		sf::Color hoverColor_;
		bool mouseHover_ = false;
	};

	/// <summary>
	/// Представляет игровую фишку
	/// </summary>
	class GUiFishka final : public GUInterface
	{
	public:
		GUiFishka(sf::Texture* texture, const sf::IntRect rect, const int id);

		const int GetID() const;

		const sf::Vector2f GetScale() const;
		void SetScale(const sf::Vector2f& scale);

		const sf::Vector2f origin;

	private:
		const sf::Vector2f defaultScale_{ .88f, .88f };
	};



	class GUiSprite final : public GUInterface
	{
	public:
		GUiSprite(sf::Texture* texture);
		void SetScale(const sf::Vector2f& scale);
	};


	/// <summary>
	/// Класс реализует интерфейс простого текстового сообщения
	/// </summary>
	class GUiText : public GUInterface
	{
	public:
		GUiText(sf::Font* font, const int id);
		GUiText(sf::Font* font, const UiType guiType, const int id);
		void SetPosition(const sf::Vector2f& pos) override;
		void SetString(const sf::String& string);
		void SetCharacterSize(const size_t size);
		void SetFillColor(const sf::Color& color);

		void SetRotation(const sf::Angle& angle) override;

		void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

		sf::Text& GetText();
		sf::Text& GetText() const;
		const sf::Color& GetColor() const;

		~GUiText();

	private:
		using GUInterface::GetRect;
		using GUInterface::GetSprite;

	protected:
		sf::Font* font_;
		sf::Text* text_;
		bool mouseHover_ = false;

		const bool Contains(const int x, const int y) const;


	};

	/// <summary>
	/// Класс реализует интерфейс текстового сообщения которое можно щелкать и наводиться мышкой на него
	/// </summary>
	class GUiLabel final : public GUiText
	{
	public:
		GUiLabel(sf::Font* font, const int labelId);

		template <typename Fn>
		void MouseDown(const sf::Event::MouseButtonEvent& downPos, Fn callBackFn);
		void MouseHover(const sf::Event::MouseMoveEvent& movPos);

	private:
		bool mouseHover_ = false;
		int labelId_;
	};

	template<typename Fn>
	inline void GUiLabel::MouseDown(const sf::Event::MouseButtonEvent& downPos, Fn callBackFn)
	{
		if (downPos.button == sf::Mouse::Button::Left && Contains(downPos.x, downPos.y))
		{
			callBackFn->MousePushCallBack(labelId_);
		}
	}

	/// <summary>
	/// Реализация элемента управления: кнопка Check
	/// </summary>
	class GUiButtonChekc final : public GUiButton
	{
	public:
		GUiButtonChekc(sf::Texture* texture, const sf::IntRect rect, const int id);

		void MouseDown(const sf::Event::MouseButtonEvent& downPos, Scene* ms) override;
		void MouseHover(const sf::Event::MouseMoveEvent& movPos) override;

	};

	/// <summary>
	/// Реализация элемента управления: кнопка New
	/// </summary>
	class GUiButtonNew final : public GUiButton
	{
	public:
		GUiButtonNew(sf::Texture* texture, const sf::IntRect rect, const int id);

		void MouseDown(const sf::Event::MouseButtonEvent& downPos, Scene* ms) override;
		void MouseHover(const sf::Event::MouseMoveEvent& movPos) override;
	};




} // namespace gamecore