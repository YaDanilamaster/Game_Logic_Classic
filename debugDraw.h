// Реализация класса Debug draw для Box2D через SFML

#ifndef SFMLDEBUGDRAW_H
#define SFMLDEBUGDRAW_H

#include <Box2D/Box2D.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>

namespace sfdd
{
	const float SCALE = 32.f;
}
namespace debugDraw {

	class SFMLDebugDraw : public b2Draw
	{
	private:
		sf::RenderWindow* m_window;
	public:
		SFMLDebugDraw(sf::RenderWindow* window);

		static sf::Color GLColorToSFML(const b2Color& color, uint8 alpha = 255)
		{
			return sf::Color(static_cast<uint8>(color.r * 255), static_cast<uint8>(color.g * 255), static_cast<uint8>(color.b * 255), alpha);
		}

		static sf::Vector2f B2VecToSFVec(const b2Vec2& vector, bool scaleToPixels = true)
		{
			return sf::Vector2f(vector.x * (scaleToPixels ? sfdd::SCALE : 1.f), vector.y * (scaleToPixels ? sfdd::SCALE : 1.f));
		}

		void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

		void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

		void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;

		void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

		void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

		void DrawTransform(const b2Transform& xf) override;

		void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

	};
} // namespace debugDraw
#endif 