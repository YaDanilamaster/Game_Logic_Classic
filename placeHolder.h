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
	/// ������������ ��������� ��� �������� � ���������� �������, � ����� ����������� ���������� � ����������� �� ���������.
	/// </summary>
	class PlaceHolder final : public sf::Drawable
	{
	public:
		using VectorSprites = std::vector<sf::Sprite*>;

		//��������, ����������� ����������� ���� ��� ����������� ������ �������� PlaceHolder
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

		// ������� ����� ������ ����������� ����� �������, ��� �������� � ��������� ����.
		// ������������ ��� �������� �����������
		PlaceHolder(const PlaceHolder& other);
		// ������ �������� ������� ������ ����� ������� GameCore
		PlaceHolder& operator=(PlaceHolder&) = delete;

		/// <summary>
		/// ������������� ��������� ������� ��������. ������ ������� �� �������� ������ ����
		/// </summary>
		/// <param name="pos"> - ���������� ��������� ������� ��������</param>
		void SetPosition(const sf::Vector2f& pos);

		/// <summary>
		/// ��������� ��������� ������������ � �������� ��������
		/// </summary>
		/// <param name="scale"> - ����������� ���������������</param>
		void SetScale(const sf::Vector2f& scale);

		/// <summary>
		/// ����������� �������������� ������ �� ������ ������� ������
		/// </summary>
		void SetInfoHolderAlignCenter();

		/// <summary>
		/// ������� ���������� ���� �������� �� ����� ������������ ���������, ������� � �����������.
		/// </summary>
		/// <param name="world"> - ��� � ������� ���������� ������� ��������</param>
		/// <param name="camera"> - ������ �� ������ ��� ���������� ������ ����������� ��������� � �����������</param>
		/// <param name="window"> - ������ �� ���� ��� ���������� ������ ����������� ��������� � �����������</param>
		void CreateHoldersBody(b2World* world, Box2DCamera* camera, sf::RenderWindow* window);

		/// <summary>
		/// ������� ����� �������� ���. ���� ������������ � ���������� �� 0 �� maxColorCount
		/// </summary>
		/// <param name="maxColorCount"> - ������������ ���������� ������</param>
		/// <param name="uniqueColor"> - ������ �� ���� ���� ����������</param>
		void CreateNewCode(const int maxColorCount, const bool uniqueColor = true);

		/// <summary>
		/// ������������ ����������� ��������� ����� � ������� ��������� ��������� ��������� ���� �� �������������� ������ ��������.
		/// </summary>
		/// <returns> - ����������� ��������� ������ ����</returns>
		/// <param name="matching_positions"> - ������ �� ������� ���������� ��������� � �������� ����</param>
		ProcessHoldersResult ProcessHolders(const bool matching_positions);

		/// <summary>
		/// �������� �������� �������� ���
		/// </summary>
		const std::vector<int>& GetCode() const;

		virtual ~PlaceHolder() override;

	private:
		PlaceHoldersTexture& mainHolder_;
		PlaceHoldersTexture& infoHolder_;
		PlaceHoldersInfoTexture& indicators_;

		// ������� ��� ��������� ��������� ����������
		VectorSprites mainHolderSprite_;
		VectorSprites infoHolderSprite_;
		VectorSprites indicatorsSprite_;
		VectorSprites itemsSprite_;

		// ������ � ��������� ����������� ����������
		VectorSprites indicatorsItem_;
		std::vector<Compliance> compilance_;

		// �������� ������ ������������� ������ ��������
		sf::Vector2f offsetMainCenter_;
		sf::Vector2f offsetInfoCenter_;

		b2World* world_;
		std::vector<b2Body*> b2HolderPosition_;						// ������ � ����������� �� ������� � ��������
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

		// ������������� ��������� ��������� � ��������� state
		void SetInfoStatus(const size_t id, const Compliance state);

		// ������� � ���������� ���� ��������� ������� ��� ���������� ������� �����
		b2Body* CreateBody(const float xh, const float yh, const b2Vec2& center);

		virtual void draw(sf::RenderTarget& target, const sf::RenderStates& states) const override;

	};



} // namespace gamecore