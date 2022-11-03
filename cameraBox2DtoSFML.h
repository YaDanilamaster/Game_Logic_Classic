#pragma once

#include "SFML/Graphics.hpp"
#include "box2d/box2d.h"

/*
¬спомогательный класс дл€ управлени€ камерой в физическом мире Box2D
и адаптации системы координат между Box2d и SFML
*/

/// <summary>
/// ”станавливает размер просматриваемой области. –азмер задаетс€ в мировых единицах Box2D
/// </summary>
/// <param name="size">размер области в представлении Box2D</param>
/// <param name="center">координаты центра камеры в мире Box2D</param>
class Box2DCamera final : public sf::View {
public:

	/// <summary>
	/// ”станавливает размер просматриваемой области. –азмер задаетс€ в мировых единицах
	/// </summary>
	/// <param name="size">размер области в представлении Box2D</param>
	/// <param name="center">координаты центра камеры в мире Box2D</param>
	/// <param name="video_mode_size">разрешение экрана</param>
	Box2DCamera(const b2Vec2& size, const b2Vec2& center, const sf::Vector2f& video_mode_size);

	/// <summary>
	/// ”станавливает новый размер просматриваемой области
	/// </summary>
	/// <param name="size">размер в единицах Box2D</param>
	void SetSize(const b2Vec2 size);

	/// <summary>
	/// ѕолучает размер видимой области в представлении Box2D
	/// </summary>
	/// <returns>размер области в единицах Box2D</returns>
	const b2Vec2& GetSize() const;

	/// <summary>
	/// ”станавливаем центр камеры в указанную позицию в мире Box2D
	/// </summary>
	/// <param name="pos_to_world">координаты в мире Box2D</param>
	void SetCenter(const b2Vec2 pos_to_world);

	/// <summary>
	/// ѕолучаем координаты центра камеры в мире Box2D
	/// </summary>
	/// <returns>координаты центра камеры</returns>
	const b2Vec2& GetCenter() const;

	/// <summary>
	/// —двигает камеру на указанное рассто€ние.
	/// </summary>
	/// <param name="move_to_in_world"></param>
	void Move(const b2Vec2& move_to);

	/// <summary>
	/// ѕолучает координаты экрана по координатам объекта из мира Box2D
	/// </summary>
	/// <param name="pos_to_world">координаты из мира Box2D</param>
	/// <returns>координаты на экране</returns>
	sf::Vector2f GetPositionInScreen(const b2Vec2& pos_to_world) const;

	/// <summary>
	/// ѕолучает позицию объекта в мире Box2D по координатам на экране
	/// </summary>
	/// <returns></returns>
	b2Vec2 GetPositionInWorld(const sf::Vector2f& pos_to_screen) const;

	/// <summary>
	///  онвертирует указанную величину из экранных единиц в пространственные
	/// </summary>
	/// <param name="value"> - величина в графических координатах</param>
	/// <returns></returns>
	b2Vec2 GetScaleFromScreenPoint(const b2Vec2& value) const;

private:
	b2Vec2 scale_{ 32.f, 32.f };
	const float deg = 57.29577f;
	b2Vec2 pos_to_world_;
	b2Vec2 size_;
	sf::Vector2f video_mode_center_;
};
