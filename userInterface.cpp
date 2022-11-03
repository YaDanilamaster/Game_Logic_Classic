#include "userInterface.h"

namespace gamecore {
	using namespace std::string_literals;

	GUInterface::GUInterface(const UiType guiType)
		: texture_(nullptr)
		, type(guiType)
	{
	}

	GUInterface::GUInterface(sf::Texture* texture, const UiType guiType)
		: texture_(texture)
		, type(guiType)
	{
		sprite_.setTexture(*texture_);
	}

	GUInterface::GUInterface(sf::Texture* texture, sf::IntRect rect, const int id, const UiType guiType)
		: texture_(texture)
		, rect_(std::move(rect))
		, id_(id)
		, type(guiType)
	{
		sprite_.setTexture(*texture_);
		sprite_.setTextureRect(rect_);
	}

	void GUInterface::SetPosition(const sf::Vector2f& pos)
	{
		sprite_.setPosition(pos);
	}

	void GUInterface::SetRotation(const sf::Angle& angle)
	{
		sprite_.setRotation(angle);
	}

	const sf::IntRect& GUInterface::GetRect() const
	{
		return sprite_.getTextureRect();
	}

	sf::Sprite& GUInterface::GetSprite()
	{
		return sprite_;
	}

	void GUInterface::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		target.draw(sprite_, states);
	}

	GUiButton::GUiButton(sf::Texture* texture, const sf::IntRect rect, const int id)
		: GUInterface(texture, rect, id, UiType::Button)
	{
	}



	const bool GUInterface::Contains(const int x, const int y) const
	{
		const sf::FloatRect& rect = sprite_.getGlobalBounds();

		sf::Vector2f pos2;
		pos2.x = rect.left + rect.width;
		pos2.y = rect.top + rect.height;

		return !(
			(pos2.y < y || pos2.x < x) ||		// вышли за нижнюю границу
			(rect.top > y || rect.left > x)			// не дошли до верхней границы
			);
	}


	GUiFishka::GUiFishka(sf::Texture* texture, const sf::IntRect rect, const int id)
		: GUInterface(texture, rect, id, UiType::Fishka)
		, origin({ 38.f, 40.f })
	{
		sprite_.setOrigin(origin);
		sprite_.setScale(defaultScale_);
	}

	const int GUiFishka::GetID() const
	{
		return id_;
	}

	const sf::Vector2f GUiFishka::GetScale() const
	{
		return sprite_.getScale();
	}

	void GUiFishka::SetScale(const sf::Vector2f& scale)
	{
		sprite_.setScale(scale);
	}


	GUiSprite::GUiSprite(sf::Texture* texture)
		: GUInterface(texture, UiType::Sprite)
	{
	}

	void GUiSprite::SetScale(const sf::Vector2f& scale)
	{
		sprite_.setScale(scale);
	}


	GUiText::GUiText(sf::Font* font, const int id)
		: GUInterface(UiType::Text)
		, font_(font)
	{
		text_ = new sf::Text;
		text_->setFont(*font_);
		id_ = id;
	}

	GUiText::GUiText(sf::Font* font, const UiType guiType, const int id)
		: GUInterface(guiType)
		, font_(font)
	{
		text_ = new sf::Text;
		text_->setFont(*font_);
		id_ = id;
	}

	void GUiText::SetPosition(const sf::Vector2f& pos)
	{
		text_->setPosition(pos);
	}

	void GUiText::SetRotation(const sf::Angle& angle)
	{
		text_->setRotation(angle);
	}

	void GUiText::SetString(const sf::String& string)
	{
		text_->setString(string);
	}

	void GUiText::SetCharacterSize(const size_t size)
	{
		text_->setCharacterSize(size);
	}

	void GUiText::SetFillColor(const sf::Color& color)
	{
		text_->setFillColor(color);
	}

	void GUiText::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		target.draw(*text_, states);
	}

	sf::Text& GUiText::GetText()
	{
		return *text_;
	}

	sf::Text& GUiText::GetText() const
	{
		return *text_;
	}

	const sf::Color& GUiText::GetColor() const
	{
		return text_->getFillColor();
	}

	GUiText::~GUiText()
	{
		delete text_;
	}

	const bool GUiText::Contains(const int x, const int y) const
	{
		const sf::FloatRect& rect = text_->getGlobalBounds();

		sf::Vector2f pos2;
		pos2.x = rect.left + rect.width;
		pos2.y = rect.top + rect.height;

		return !(
			(pos2.y < y || pos2.x < x) ||		// вышли за нижнюю границу
			(rect.top > y || rect.left > x)			// не дошли до верхней границы
			);
	}

	GUiLabel::GUiLabel(sf::Font* font, const int labelId)
		: GUiText(font, UiType::Label, labelId)
		, labelId_(labelId)
	{
	}

	void GUiLabel::MouseHover(const sf::Event::MouseMoveEvent& movPos)
	{
		mouseHover_ = Contains(movPos.x, movPos.y);

		if (mouseHover_) {
			text_->setFillColor(ColorStyle.highlighting);
		}
		else {
			text_->setFillColor(ColorStyle.base);
		}
	}


	GUiButtonChekc::GUiButtonChekc(sf::Texture* texture, const sf::IntRect rect, const int id)
		: GUiButton(texture, rect, id)
	{
		defaultColor_ = { 200, 200, 200, 255 };
		hoverColor_ = { 255, 255, 255, 255 };
		sprite_.setColor(defaultColor_);
	}


	void GUiButtonChekc::MouseHover(const sf::Event::MouseMoveEvent& movPos)
	{
		mouseHover_ = Contains(movPos.x, movPos.y);

		if (mouseHover_) {
			sprite_.setColor(hoverColor_);
		}
		else {
			sprite_.setColor(defaultColor_);
		}
	}

	void GUiButtonChekc::MouseDown(const sf::Event::MouseButtonEvent& downPos, Scene* ms)
	{
		if (Contains(downPos.x, downPos.y))
		{
			ms->MousePushCallBack(id_);
		}
	}


	GUiButtonNew::GUiButtonNew(sf::Texture* texture, const sf::IntRect rect, const int id)
		: GUiButton(texture, rect, id)
	{
		defaultColor_ = { 200, 200, 200, 255 };
		hoverColor_ = { 255, 255, 255, 255 };
		sprite_.setColor(defaultColor_);
	}

	void GUiButtonNew::MouseDown(const sf::Event::MouseButtonEvent& downPos, Scene* ms)
	{
		if (Contains(downPos.x, downPos.y))
		{
			ms->MousePushCallBack(id_);
		}
	}

	void GUiButtonNew::MouseHover(const sf::Event::MouseMoveEvent& movPos)
	{
		mouseHover_ = Contains(movPos.x, movPos.y);

		if (mouseHover_) {
			sprite_.setColor(hoverColor_);
		}
		else {
			sprite_.setColor(defaultColor_);
		}
	}

} // namespace gamecore