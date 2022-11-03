#pragma once

#include "SFML/Graphics.hpp"
#include "box2d/box2d.h"

/*
��������������� ����� ��� ���������� ������� � ���������� ���� Box2D
� ��������� ������� ��������� ����� Box2d � SFML
*/

/// <summary>
/// ������������� ������ ��������������� �������. ������ �������� � ������� �������� Box2D
/// </summary>
/// <param name="size">������ ������� � ������������� Box2D</param>
/// <param name="center">���������� ������ ������ � ���� Box2D</param>
class Box2DCamera final : public sf::View {
public:

	/// <summary>
	/// ������������� ������ ��������������� �������. ������ �������� � ������� ��������
	/// </summary>
	/// <param name="size">������ ������� � ������������� Box2D</param>
	/// <param name="center">���������� ������ ������ � ���� Box2D</param>
	/// <param name="video_mode_size">���������� ������</param>
	Box2DCamera(const b2Vec2& size, const b2Vec2& center, const sf::Vector2f& video_mode_size);

	/// <summary>
	/// ������������� ����� ������ ��������������� �������
	/// </summary>
	/// <param name="size">������ � �������� Box2D</param>
	void SetSize(const b2Vec2 size);

	/// <summary>
	/// �������� ������ ������� ������� � ������������� Box2D
	/// </summary>
	/// <returns>������ ������� � �������� Box2D</returns>
	const b2Vec2& GetSize() const;

	/// <summary>
	/// ������������� ����� ������ � ��������� ������� � ���� Box2D
	/// </summary>
	/// <param name="pos_to_world">���������� � ���� Box2D</param>
	void SetCenter(const b2Vec2 pos_to_world);

	/// <summary>
	/// �������� ���������� ������ ������ � ���� Box2D
	/// </summary>
	/// <returns>���������� ������ ������</returns>
	const b2Vec2& GetCenter() const;

	/// <summary>
	/// �������� ������ �� ��������� ����������.
	/// </summary>
	/// <param name="move_to_in_world"></param>
	void Move(const b2Vec2& move_to);

	/// <summary>
	/// �������� ���������� ������ �� ����������� ������� �� ���� Box2D
	/// </summary>
	/// <param name="pos_to_world">���������� �� ���� Box2D</param>
	/// <returns>���������� �� ������</returns>
	sf::Vector2f GetPositionInScreen(const b2Vec2& pos_to_world) const;

	/// <summary>
	/// �������� ������� ������� � ���� Box2D �� ����������� �� ������
	/// </summary>
	/// <returns></returns>
	b2Vec2 GetPositionInWorld(const sf::Vector2f& pos_to_screen) const;

	/// <summary>
	/// ������������ ��������� �������� �� �������� ������ � ����������������
	/// </summary>
	/// <param name="value"> - �������� � ����������� �����������</param>
	/// <returns></returns>
	b2Vec2 GetScaleFromScreenPoint(const b2Vec2& value) const;

private:
	b2Vec2 scale_{ 32.f, 32.f };
	const float deg = 57.29577f;
	b2Vec2 pos_to_world_;
	b2Vec2 size_;
	sf::Vector2f video_mode_center_;
};
