#include "cameraBox2DtoSFML.h"

Box2DCamera::Box2DCamera(const b2Vec2& size, const b2Vec2& center, const sf::Vector2f& video_mode_size)
	: pos_to_world_(center), size_(size)
	, video_mode_center_(video_mode_size / 2.f)
{
	SetSize(size);
	SetCenter(center);
}

void Box2DCamera::SetSize(const b2Vec2 size)
{
	size_ = size;
	View::setSize({ size_.x * scale_.x, size.y * scale_.y });
}

const b2Vec2& Box2DCamera::GetSize() const
{
	return size_;
}

void Box2DCamera::SetCenter(const b2Vec2 pos_to_world)
{
	pos_to_world_ = pos_to_world;
	View::setCenter({ pos_to_world.x * scale_.x, pos_to_world.y * scale_.y });
}

const b2Vec2& Box2DCamera::GetCenter() const
{
	return pos_to_world_;
}

void Box2DCamera::Move(const b2Vec2& move_to)
{
	View::move({ move_to.x * scale_.x, move_to.y * scale_.y });
	pos_to_world_ += move_to;
}

sf::Vector2f Box2DCamera::GetPositionInScreen(const b2Vec2& pos_to_world) const
{
	b2Vec2 result = pos_to_world - pos_to_world_;
	return {
		video_mode_center_.x + result.x * scale_.x,
		video_mode_center_.y + result.y * scale_.y
	};
}

b2Vec2 Box2DCamera::GetPositionInWorld(const sf::Vector2f& pos_to_screen) const
{
	sf::Vector2f result = pos_to_screen - video_mode_center_;
	return {
		pos_to_world_.x + result.x / scale_.x,
		pos_to_world_.y + result.y / scale_.y
	};
}

b2Vec2 Box2DCamera::GetScaleFromScreenPoint(const b2Vec2& value) const
{
	return { value.x / scale_.x, value.y / scale_.y };
}
