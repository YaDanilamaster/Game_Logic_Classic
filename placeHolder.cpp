#include "placeHolder.h"

namespace gamecore {

	std::ostream& operator<<(std::ostream& os, std::vector<Compliance> cmp) {
		for (Compliance item : cmp) {
			switch (item)
			{
			case Compliance::Full:
				os << "1 ";
				break;
			case Compliance::Partial:
				os << "0 ";
				break;
			case Compliance::None:
				os << "- ";
				break;
			default:
				break;
			}
		}
		os << std::endl;
		return os;
	}


	PlaceHolder::PlaceHolder(
		PlaceHoldersTexture& mainHolder,
		PlaceHoldersTexture& infoHolder,
		PlaceHoldersInfoTexture& indicators,
		size_t placeCount
	)
		: mainHolder_(mainHolder), infoHolder_(infoHolder)
		, indicators_(indicators)
		, placeCount_(placeCount)
	{
		Construct();
	}

	PlaceHolder::PlaceHolder(const PlaceHolder& other)
		: mainHolder_(other.mainHolder_)
		, infoHolder_(other.infoHolder_)
		, indicators_(other.indicators_)
		, placeCount_(other.placeCount_)
		, indicatorsItem_(other.indicatorsItem_)
		, compilance_(other.compilance_)
	{
		Construct();

		// скопировано все необходимое, кроме фишек
		itemsSprite_.resize(placeCount_);
		b2Body* sensor;

		for (size_t i = 0; i < placeCount_; i++)
		{
			sensor = other.b2HolderPosition_[i];
			B2BodyUserDataD* userData = reinterpret_cast<B2BodyUserDataD*>(sensor->GetUserData().pointer);

			if (userData->cash == 1) {
				sf::FloatRect mainRect = mainHolderSprite_[i + 1]->getGlobalBounds();
				const sf::IntRect& stateRect = static_cast<GUiFishka*>(userData->itemPointer)->GetRect();
				sf::Sprite* newFishka = new sf::Sprite(static_cast<GUiFishka*>(userData->itemPointer)->GetSprite());

				newFishka->setPosition(
					{ mainRect.left + (mainRect.width - stateRect.width) / 2, mainRect.top + (mainRect.height - stateRect.height) / 2.6f }
				);
				itemsSprite_[i] = newFishka;
			}

			SetInfoStatus(i, compilance_[i]);
		}
	}

	void PlaceHolder::SetPosition(const sf::Vector2f& pos)
	{
		mainRect_ = SetHolderPosition(mainHolderSprite_, pos);
		infoRect_ = SetHolderPosition(infoHolderSprite_, { pos.x + 50, pos.y - 100 });
		SetInfoHolderAlignCenter();
		SetItemsPosition();
	}

	void PlaceHolder::SetScale(const sf::Vector2f& scale)
	{
		SetHoldersScale(mainHolderSprite_, scale);
		SetHoldersScale(infoHolderSprite_, scale);
		SetHoldersScale(indicatorsSprite_, scale);
		SetHoldersScale(itemsSprite_, scale);

		// после изменения масштаба необходимо обновить позиции всех спрайтов
		SetPosition(pos_);
	}

	void PlaceHolder::SetInfoHolderAlignCenter()
	{
		sf::Vector2f centerPos;
		centerPos.x = (mainRect_.left + mainRect_.width / 2) - infoRect_.width / 2;
		centerPos.y = mainRect_.top - infoRect_.height * 0.8;
		infoRect_ = SetHolderPosition(infoHolderSprite_, centerPos);
		SetStatePosition(centerPos);
	}


	void PlaceHolder::CreateHoldersBody(b2World* world, Box2DCamera* camera, sf::RenderWindow* window)
	{
		world_ = world;
		camera_ = camera;
		window_ = window;
		sf::FloatRect rect;
		b2Vec2 center;
		b2Vec2 size;

		// Создаем держатели фишек (они же сенсоры)
		for (size_t i = 0; i < placeCount_; i++) {
			rect = mainHolderSprite_[i + 1]->getGlobalBounds();
			size = camera->GetScaleFromScreenPoint({ rect.width, rect.height });
			center = camera->GetPositionInWorld({rect.left + rect.width / 2, rect.top + rect.height / 2});
			b2HolderPosition_.push_back(CreateBody(size.x, size.y, center));
		}
	}

	void PlaceHolder::CreateNewCode(const int maxColorCount, const bool uniqueColor)
	{
		if (logic == nullptr) {
			logic = new Logic(placeCount_, maxColorCount);
		}

		logic->CreateNewCode(uniqueColor);
	}

	ProcessHoldersResult PlaceHolder::ProcessHolders(const bool matching_positions)
	{
		std::vector<int> userCode(placeCount_);
		b2Body* sensor;

		for (size_t i = 0; i < placeCount_; i++)
		{
			sensor = b2HolderPosition_[i];
			B2BodyUserDataD* userData = reinterpret_cast<B2BodyUserDataD*>(sensor->GetUserData().pointer);

			if (userData->cash == 1) {
				userCode[i] = static_cast<GUiFishka*>(userData->itemPointer)->GetID();
			}
			else {
				return ProcessHoldersResult::Failed;
			}
		}

		compilance_ = logic->GetCodeBreake(userCode, matching_positions);
		allComplite_ = true;
		//std::cout << compilance_;

		for (size_t i = 0; i < placeCount_; i++)
		{
			SetInfoStatus(i, compilance_[i]);
			if (compilance_[i] != Compliance::Full) allComplite_ = false;
		}

		if (allComplite_) return ProcessHoldersResult::Hacked;
		return ProcessHoldersResult::Hold;
	}

	PlaceHolder::~PlaceHolder()
	{
		for (auto i : mainHolderSprite_)
		{
			delete i;
		}

		for (auto j : infoHolderSprite_)
		{
			delete j;
		}

		for (auto k : b2UserDataPtr_)
		{
			delete k;
		}

		for (auto l : indicatorsSprite_)
		{
			delete l;
		}


		if (logic) delete logic;
	}

	void PlaceHolder::CreatePlaces(
		PlaceHoldersTexture& holderTexture,
		VectorSprites& places,
		const sf::Vector2f& scale,
		const sf::Color& color)
	{
		places.reserve(placeSprites_);

		sf::Sprite* sprite1 = new sf::Sprite(*holderTexture.leftBorder);
		sprite1->setScale(scale);
		sprite1->setColor(color);
		places.push_back(sprite1);

		for (size_t i = 2; i < placeSprites_; i++) {
			sf::Sprite* sprite2 = new sf::Sprite(*holderTexture.centerBorder);
			sprite2->setScale(scale);
			sprite2->setColor(color);
			places.push_back(sprite2);
		}

		sf::Sprite* sprite3 = new sf::Sprite(*holderTexture.rightBorder);
		sprite3->setScale(scale);
		sprite3->setColor(color);
		places.push_back(sprite3);
	}

	void PlaceHolder::Construct()
	{
		// Боковые рамки и одно место, это минимум для отображения на экране
		placeCount_ = placeCount_ < 1 ? 1 : placeCount_;
		placeSprites_ = placeCount_ + 2;

		CreatePlaces(mainHolder_, mainHolderSprite_, { 1.f, 1.f }, { 255, 255, 255, 255 });
		CreatePlaces(infoHolder_, infoHolderSprite_, { 0.3f, 0.3f }, { 255, 255, 255, 155 });

		sf::Vector2u size = mainHolder_.centerBorder->getSize();
		offsetMainCenter_ = { size.x / 2.f, size.y / 2.f };

		size = infoHolder_.centerBorder->getSize();
		offsetInfoCenter_ = { size.x / 2.f, size.y / 2.f };

		indicatorsSprite_.reserve(placeCount_);
		CreateIndicators({ 0.3f, 0.3f }, { 255, 255, 255, 155 });
	}

	void PlaceHolder::CreateIndicators(const sf::Vector2f& scale, const sf::Color& color)
	{
		indicatorsItem_.reserve(3);

		{
			sf::Sprite* sprite = new sf::Sprite(*indicators_.full);
			sprite->setScale(scale);
			sprite->setColor(color);
			indicatorsItem_.push_back(sprite);
		}
		{
			sf::Sprite* sprite = new sf::Sprite(*indicators_.half);
			sprite->setScale(scale);
			sprite->setColor(color);
			indicatorsItem_.push_back(sprite);
		}
		{
			sf::Sprite* sprite = new sf::Sprite(*indicators_.empty);
			sprite->setScale(scale);
			sprite->setColor(color);
			indicatorsItem_.push_back(sprite);
		}

		for (size_t i = 0; i < placeCount_; i++) {
			sf::FloatRect rect = infoHolderSprite_[i + 1]->getGlobalBounds();
			sf::Sprite* sprite = new sf::Sprite(*indicatorsItem_[2]);
			sprite->setPosition({ rect.left, rect.top });
			indicatorsSprite_.push_back(sprite);
		}

	}

	sf::FloatRect PlaceHolder::SetHolderPosition(VectorSprites& places, const sf::Vector2f& pos)
	{
		pos_ = pos;
		places[0]->setPosition(pos_);
		sf::FloatRect rect;
		size_t i = 1;

		sf::FloatRect resultRect;
		resultRect.left = pos_.x;
		resultRect.top = pos_.y;
		resultRect.width = 0.f;

		for (; i < placeSprites_; i++) {
			rect = places[i - 1]->getGlobalBounds();
			places[i]->setPosition({ rect.left + rect.width, rect.top });
			resultRect.width += rect.width;
		}

		resultRect.width += places[i - 1]->getGlobalBounds().width;
		resultRect.height = places[i - 1]->getGlobalBounds().height;

		return resultRect;
	}

	void PlaceHolder::SetStatePosition(const sf::Vector2f& pos)
	{
		const sf::FloatRect& stateRect = indicatorsSprite_[0]->getGlobalBounds();

		for (size_t i = 0; i < placeCount_; i++) {
			sf::FloatRect infoRect = infoHolderSprite_[i + 1]->getGlobalBounds();

			indicatorsSprite_[i]->setPosition(
				{ infoRect.left + (infoRect.width - stateRect.width) / 2, infoRect.top + (infoRect.height - stateRect.height) / 2.6f }
			);
		}
	}

	void PlaceHolder::SetItemsPosition()
	{
		for (size_t i = 0; i < itemsSprite_.size(); i++)
		{
			const sf::FloatRect& mainRect = mainHolderSprite_[i + 1]->getGlobalBounds();
			const sf::FloatRect& itemRect = indicatorsSprite_[i]->getGlobalBounds();

			itemsSprite_[i]->setPosition(
				{ mainRect.left + mainRect.width / 2 , mainRect.top + mainRect.height / 2 }
			);
		}

	}

	void PlaceHolder::SetHoldersScale(VectorSprites& places, const sf::Vector2f& scale)
	{
		for (sf::Sprite* sp : places) {
			sp->scale(scale);
		}
	}

	void PlaceHolder::SetInfoStatus(const size_t id, const Compliance state)
	{
		indicatorsSprite_[id]->setTexture(*indicatorsItem_[static_cast<int>(state)]->getTexture());
	}

	b2Body* PlaceHolder::CreateBody(const float xh, const float yh, const b2Vec2& center)
	{
		// прикрепляем сенсор
		b2BodyDef bd;
		bd.position.Set(center.x, center.y);
		b2Body* sens = world_->CreateBody(&bd);

		b2CircleShape shape;
		shape.m_radius = 1.2f;

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.isSensor = true;
		sens->CreateFixture(&fd);

		B2BodyUserDataD* userData = new B2BodyUserDataD;
		b2UserDataPtr_.push_back(userData);

		userData->itemType = UiType::Sensor;
		userData->isDrawable = false;
		userData->cash = 0;

		sens->GetUserData().pointer = reinterpret_cast<uintptr_t>(userData);

		return sens;
	}

	void PlaceHolder::draw(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		for (sf::Sprite* isp : infoHolderSprite_) {
			target.draw(*isp, states);
		}

		for (sf::Sprite* msp : mainHolderSprite_) {
			target.draw(*msp, states);
		}

		for (sf::Sprite* state : indicatorsSprite_) {
			target.draw(*state, states);
		}

		for (sf::Sprite* items : itemsSprite_) {
			target.draw(*items, states);
		}
	}


	const std::vector<int>& PlaceHolder::GetCode() const
	{
		return logic->GetCode();
	}

} //namespace gamecore