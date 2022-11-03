#include "mainScene.h"

namespace gamecore {

	MainScene::MainScene(b2Vec2&& gravity, sf::RenderWindow* window)
		: Scene(window)
	{
		// настраиваем новый мир
		mainLevel_ = new b2World(std::move(gravity));
		// bodyDef нужен дл€ прив€зки к мышки, как точка заземлени€
		b2BodyDef bodyDef;
		m_groundBody_ = mainLevel_->CreateBody(&bodyDef);

		// настраиваем камеру
		camera_ = new Box2DCamera({ 40, 28 }, { 20, 14 }, sf::Vector2f{ 1280, 900 });

		// »нициируем дебаг рисовалку дл€ физических границ
		debugDrawInstance_ = new debugDraw::SFMLDebugDraw(window_);

		// прив€зываем отладочную рисовалку к конкретному миру
		mainLevel_->SetDebugDraw(debugDrawInstance_);
		// устанавливаем флаги дл€ необходимой отладочной информации
		uint32 flags = 0;
		//flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_jointBit;
		//flags += b2Draw::e_aabbBit;
		//flags += b2Draw::e_centerOfMassBit;
		debugDrawInstance_->SetFlags(flags);

		// включаем камеру
		defaultSize_ = sf::Vector2u{ 1280, 900 };

		scale_ = {1.f, 1.f};
		window_->setView(*camera_);

		// настраиваем систему событий
		eventsGame_ = new EventHandler(m_groundBody_);
		sensor_ = new Sensor(*eventsGame_);
		mainLevel_->SetContactListener(sensor_);
	}

	void MainScene::InitMainWorld(const int difficulty)
	{
		difficulty_ = difficulty;

		// задаем начальные параметры первого игрового шага
		stepCore_.stepCount = 0;
		stepCore_.camera = camera_;
		stepCore_.scene = this;
		stepCore_.eventHandler = eventsGame_;
		stepCore_.world = mainLevel_;
		stepCore_.prevDuration = 0;
		stepCore_.curentDuration = 0;

		// главна€ коробка
		CreateWall(20, 0.1f, { 20, 0 }, 0);
		CreateWall(20, 0.1f, { 20, 28 }, 0);
		CreateWall(0.1f, 14, { 0, 14 }, 0);
		CreateWall(0.1f, 14, { 40, 14 }, 0);

		// загружаем текстуры
		LoadTexture();

		// создаем графические элементы

		Scene::CreateGUInterface<GUiSprite>(0, false, GetTexture((int)Texture::BackGround));
		
		const int centerBorderX = 685;

		{
			// центральна€ граница
			CreateWall(0.4f, 14, { 21, 14 }, 0);
			auto border = Scene::CreateGUInterface<GUiSprite>(1, false, GetTexture((int)Texture::Border));
			border->SetPosition({ centerBorderX, 0});
			border->SetScale({window_->getSize().y * 1.f / border->GetRect().width, 1});
			border->SetRotation(sf::radians(1.57f));
		}
		{
			// стакан дл€ фишек
			CreateWall(0.1f, 2.5f, { 1.2f, 25 }, -0.3);
			CreateWall(0.1f, 2.5f, { 12.9f, 25 }, 0.3);

			auto glas1 = Scene::CreateGUInterface<GUiSprite>(1, false, GetTexture((int)Texture::Glass));
			glas1->SetPosition({ -15, 670 });
			glas1->SetScale({ 0.8f, 0.4f });
		}

		{
			// насыпаем нужное количество фишек
			stepCore_.eventHandler->AddEvent(
				std::make_unique<SpawnFishka>(MakeGUiFishka(COLOR_COUNT_MAX * 2, COLOR_COUNT_MAX - 1), b2Vec2{ 10, 14 }, 3000), EventType::Spawn);
		}

		// настраиваем элементы управлени€
		{
			{
				GUiLabel* msg = CreateGUInterface<GUiLabel>
					(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::BackMenu);

				msg->SetString(L"¬ меню");
				msg->SetCharacterSize(50);
				msg->SetFillColor(ColorStyle.base);
				sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
				msg->SetPosition({ centerBorderX + 10, 50 });
			}
			{
				GUiLabel* msg = CreateGUInterface<GUiLabel>
					(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Retreat);

				msg->SetString(L"—даюсь...");
				msg->SetCharacterSize(50);
				msg->SetFillColor(ColorStyle.base);
				sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
				msg->SetPosition({ centerBorderX + 10, 100 });
				labelRetreatPrt_ = msg;
			}
			{
				GUiLabel* msg = CreateGUInterface<GUiLabel>
					(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::NewGame);

				msg->SetString(L"Ќачать с начала");
				msg->SetCharacterSize(50);
				msg->SetFillColor(ColorStyle.base);
				sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
				msg->SetPosition({ centerBorderX + 10, 150 });
			}
			{
				GUiLabel* msg = CreateGUInterface<GUiLabel>
					(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Check);

				msg->SetString(L"ѕроверить");
				msg->SetCharacterSize(50);
				msg->SetFillColor(ColorStyle.base);
				sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
				msg->SetPosition({ centerBorderX + 10, 250 });
				labelCheckPrt_ = msg;
			}
			{
				auto* info = Scene::CreateGUInterface<GUiText>(1, false, GetFont((int)Font::Message), -1);

				info->SetString(L"—ложность: " + difficultyToString.at(difficulty_));
				info->SetCharacterSize(30);
				info->SetFillColor(ColorStyle.blocked);
				info->SetPosition({ centerBorderX + 10, 850});
			}
		}

		{
			// упаковываем текстуры в спомогательные структуры
			mainPlaceTexture_ = new PlaceHolder::PlaceHoldersTexture
			{
				GetTexture((int)Texture::PlaceHoldersLeft),
				GetTexture((int)Texture::PlaceHoldersCenter),
				GetTexture((int)Texture::PlaceHoldersRight),
			};

			infoPlaceTexture_ = new PlaceHolder::PlaceHoldersTexture
			{
				GetTexture((int)Texture::PlaceHoldersInfoLeft),
				GetTexture((int)Texture::PlaceHoldersInfoCenter),
				GetTexture((int)Texture::PlaceHoldersInfoRight),
			};

			statePlaceTexture_ = new PlaceHolder::PlaceHoldersInfoTexture
			{
				GetTexture((int)Texture::PlaceHoldersEmpty),
				GetTexture((int)Texture::PlaceHoldersHalf),
				GetTexture((int)Texture::PlaceHoldersFull),
			};

			mainPlaceHolder_ = Scene::CreateGUInterface<PlaceHolder>
				(2, false, *mainPlaceTexture_, *infoPlaceTexture_, *statePlaceTexture_, COLOR_COUNT);
			mainPlaceHolder_->SetPosition({ -70, 250 });
			mainPlaceHolder_->SetScale({ 1.0f, 1.0f });
			mainPlaceHolder_->CreateHoldersBody(mainLevel_, camera_, window_);

			// »нициируем новый цветовой код
			mainPlaceHolder_->CreateNewCode(COLOR_COUNT_MAX, (difficulty_ < 2));
		}

		tryCount_ = 0;
	}

	void MainScene::ProcessHolders()
	{
		switch (mainPlaceHolder_->ProcessHolders(difficulty_ < 1))
		{
		case gamecore::ProcessHoldersResult::Failed:
			FailedProcess();
			break;
		case gamecore::ProcessHoldersResult::Hold:
			if (tryPosition_.size() > 0) {
				TryInfo();
			}

			if (tryPosition_.size() == 0) {
				GameOver();
			}
			break;
		case gamecore::ProcessHoldersResult::Hacked:
			YouWin();
			break;
		default:
			break;
		}
	}

	void MainScene::BlockMenuItems()
	{
		eventsMouseMoved_.Unsubscribe(labelCheckPrt_, &GUiLabel::MouseHover);
		eventsMouseDown_.Unsubscribe(labelCheckPrt_, &GUiLabel::MouseDown);
		labelCheckPrt_->SetFillColor(ColorStyle.blocked);

		eventsMouseMoved_.Unsubscribe(labelRetreatPrt_, &GUiLabel::MouseHover);
		eventsMouseDown_.Unsubscribe(labelRetreatPrt_, &GUiLabel::MouseDown);
		labelRetreatPrt_->SetFillColor(ColorStyle.blocked);
	}

	void MainScene::MousePushCallBack(const int labelId)
	{
		switch (static_cast<MenuItem>(labelId))
		{
		case MenuItem::BackMenu:
			exitCode_ = ExitSceneCode::NewScene;
			break;

		case MenuItem::NewGame:
			exitCode_ = ExitSceneCode::Restart;
			break;

		case MenuItem::Check:
			ProcessHolders();
			break;

		case MenuItem::Retreat:
			BlockMenuItems();

			stepCore_.eventHandler->AddEvent(std::make_unique<ShowCode>
				(MakeGUiFishka(mainPlaceHolder_->GetCode()), 300), EventType::Show);
			gameOver_ = true;
			break;

		default:
			break;
		}
	}

	void MainScene::FailedProcess()
	{
		GUiText* message = new GUiText(GetFont((int)Font::Message), 100);
		message->SetString(L"Ќе полный код, нужно больше информации...");
		message->GetText().setLetterSpacing(1.5f);
		message->SetCharacterSize(50);
		message->SetFillColor({ 255, 191, 0, 255 });

		stepCore_.eventHandler->AddEvent(std::make_unique<ShowMessage>
			(message, 2000), EventType::Messadge);
	}

	// ѕоказываем текущую попытку в истории событий
	void MainScene::TryInfo()
	{
		++tryCount_;
		sf::Vector2f& tryPos = tryPosition_.back();
		auto pHolder = Scene::CreateGUInterface<PlaceHolder>(3, false, *mainPlaceHolder_);
		pHolder->SetPosition(tryPos);
		pHolder->SetScale({ 0.35f, 0.35f });

		sf::Text* message = new sf::Text;
		message->setString(std::to_string(tryCount_));
		message->setFont(*GetFont((int)Font::Message));
		message->setCharacterSize(40);
		message->setFillColor({ 255, 191, 0, 255 });
		message->setPosition(tryPos + sf::Vector2f{ 50.f, -60.f });
		Scene::CreateGUInterface<sf::Text>(4, true, *message);

		tryPosition_.resize(tryPosition_.size() - 1);
	}

	void MainScene::GameOver()
	{
		BlockMenuItems();

		{
			GUiText* message = new GUiText(GetFont((int)Font::Message), 100);
			message->SetString(L"  ќ неееееет!!!");
			message->GetText().setLetterSpacing(2);
			message->SetCharacterSize(80);
			message->SetFillColor({ 255, 191, 0, 255 });
			message->GetText().setStyle(sf::Text::Bold);

			// запускаем событие "ќ нет!" и показ исходного кода
			stepCore_.eventHandler->AddEvent(std::make_unique<ShowMessage>
				(message, 1500), EventType::Messadge);

			stepCore_.eventHandler->AddEvent(std::make_unique<ShowCode>
				(MakeGUiFishka(mainPlaceHolder_->GetCode()), 6000), EventType::Show);
			gameOver_ = true;
		}

		{
			GUiText* message = new GUiText(GetFont((int)Font::Message), 100);
			message->SetString(L"»гра окончена :(");
			message->GetText().setLetterSpacing(1.5f);
			message->SetCharacterSize(100);
			message->SetFillColor({ 255, 0, 0, 255 });
			message->GetText().setStyle(sf::Text::Bold);

			// запускаем событие "игра окончена"
			stepCore_.eventHandler->AddEvent(std::make_unique<ShowMessage>
				(message, 5000, 1000), EventType::Messadge);
		}
	}

	void MainScene::YouWin()
	{
		BlockMenuItems();

		GUiText* message = new GUiText(GetFont((int)Font::Message), 100);
		message->SetString(L"ћои поздравлени€!  од взломан!");
		message->GetText().setLetterSpacing(2);
		message->SetCharacterSize(60);
		message->SetFillColor({ 255, 191, 0, 255 });
		message->GetText().setStyle(sf::Text::Bold);

		// запускаем событи€ " од взломан! ..." и показ исходного кода
		stepCore_.eventHandler->AddEvent(std::make_unique<ShowMessage>
			(message, 10000), EventType::Messadge);

		stepCore_.eventHandler->AddEvent(std::make_unique<ShowCode>
			(MakeGUiFishka(mainPlaceHolder_->GetCode()), 11000), EventType::Show);
		gameOver_ = true;

	}

	void MainScene::MainLoop()
	{
		auto start_time = std::chrono::steady_clock::now();
		sf::Vector2f pos;
		while (window_->isOpen() && exitCode_ == ExitSceneCode::None)
		{
			std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

			sf::Event event;
			while (window_->pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
					window_->close();
					exitCode_ = ExitSceneCode::CloseWindow;
					break;

				case sf::Event::MouseButtonPressed:
					pos = window_->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
					MouseDown(camera_->GetPositionInWorld(pos));
					eventsMouseDown_(sf::Event::MouseButtonEvent{ event.mouseButton.button, (int)pos.x, (int)pos.y }, this);
					break;

				case sf::Event::MouseMoved:
					pos = window_->mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
					MouseMove(camera_->GetPositionInWorld(pos));
					eventsMouseMoved_(sf::Event::MouseMoveEvent{ (int)pos.x, (int)pos.y });
					break;

				case sf::Event::MouseButtonReleased:
					pos = window_->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
					MouseUp(camera_->GetPositionInWorld(pos));
					break;

				}
			}

			eventsGame_->ProcessEvent(stepCore_);		// обрабатываем событи€ 

			mainLevel_->Step(WORLDSTEEP, 8, 3);			// выполн€ем расчет физики

			window_->clear();
			DrawGUI();
			mainLevel_->DebugDraw();					// отрисовываем отладочную информацию из физического движка
			window_->display();

			++stepCore_.stepCount;
			stepCore_.prevDuration = stepCore_.curentDuration;

			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			stepCore_.curentDuration = std::chrono::duration_cast<std::chrono::milliseconds>((t2 - t1)).count();	// ms
		}
	}

	MainScene::~MainScene()
	{
		delete mainLevel_;
		delete camera_;
		delete debugDrawInstance_;

		//delete m_mouseJoint_;
		if (sensor_) delete sensor_;

		delete mainPlaceTexture_;
		delete infoPlaceTexture_;
		delete statePlaceTexture_;
	}

	void MainScene::CreateWall(const float xh, const float yh, const b2Vec2& center, float angle)
	{
		b2PolygonShape shape;
		shape.SetAsBox(xh, yh, center, angle);

		b2FixtureDef fd;
		fd.shape = &shape;

		b2BodyDef bd;
		bd.type = b2_staticBody;

		b2Body* body = mainLevel_->CreateBody(&bd);
		body->CreateFixture(&fd);
	}

	b2Body* MainScene::CreateBodyFishka(const float x, const float y)
	{
		b2CircleShape circle;
		circle.m_radius = 1;

		b2FixtureDef fd;
		fd.shape = &circle;
		fd.density = 10.0f;
		fd.restitution = 0.4f;

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(x, y);

		b2Body* body = mainLevel_->CreateBody(&bd);
		body->CreateFixture(&fd);
		return body;
	}

	void MainScene::MouseDown(const b2Vec2& p)
	{
		m_mouseWorld_ = p;

		if (m_mouseJoint_ != NULL)
		{
			return;
		}

		// —оздаем маленькую рамку.
		b2AABB aabb{};
		b2Vec2 d{};
		d.Set(0.001f, 0.001f);
		aabb.lowerBound = p - d;
		aabb.upperBound = p + d;

		// Query the world for overlapping shapes.
		QueryCallback callback(p);
		mainLevel_->QueryAABB(&callback, aabb);

		if (callback.m_fixture)
		{
			float frequencyHz = 5.0f;
			float dampingRatio = 0.7f;

			b2Body* body = callback.m_fixture->GetBody();
			b2MouseJointDef jd;
			jd.bodyA = m_groundBody_;

			jd.bodyB = body;
			jd.target = p;
			jd.maxForce = 1000.0f * body->GetMass();
			b2LinearStiffness(jd.stiffness, jd.damping, frequencyHz, dampingRatio, jd.bodyA, jd.bodyB);

			m_mouseJoint_ = (b2MouseJoint*)mainLevel_->CreateJoint(&jd);
			body->SetAwake(true);
		}
	}

	void MainScene::MouseMove(const b2Vec2& p)
	{
		m_mouseWorld_ = p;
		if (m_mouseJoint_)
		{
			m_mouseJoint_->SetTarget(p);
		}
	}

	void MainScene::MouseUp(const b2Vec2& p)
	{
		if (m_mouseJoint_)
		{
			mainLevel_->DestroyJoint(m_mouseJoint_);
			m_mouseJoint_ = NULL;
		}
	}

	std::vector<GUiFishka*> MainScene::MakeGUiFishka(const int count, const int maxId)
	{
		std::vector<GUiFishka*> result;
		result.reserve(count);
		int id = 0;

		for (size_t i = 0; i < count; i++) {
			auto f = Scene::CreateGUInterface<GUiFishka>
				(5, false, GetTexture((int)Texture::Fishka), TextureFishkaByIndex[id], id);
			f->SetPosition({ -100, -100 });
			result.push_back(f);

			id = id == maxId ? 0 : ++id;
		}

		return result;
	}

	std::vector<GUiFishka*> MainScene::MakeGUiFishka(const std::vector<int>& items)
	{
		std::vector<GUiFishka*> result;
		result.reserve(items.size());
		int id = 0;

		for (const int id : items) {
			auto f = Scene::CreateGUInterface<GUiFishka>
				(5, false, GetTexture((int)Texture::Fishka), TextureFishkaByIndex[id], id);
			result.push_back(f);
		}

		return result;
	}

	void MainScene::LoadTexture()
	{
		AddTexture("img/back_ground.jpg"s, (int)Texture::BackGround);
		AddTexture("img/glass1.png"s, (int)Texture::Glass);
		AddTexture("img/glass_orbs.png"s, (int)Texture::Fishka);

		AddTexture("img/mainPlace1.png"s, (int)Texture::PlaceHoldersLeft);
		AddTexture("img/mainPlace2.png"s, (int)Texture::PlaceHoldersCenter);
		AddTexture("img/mainPlace3.png"s, (int)Texture::PlaceHoldersRight);

		AddTexture("img/infoPlace21.png"s, (int)Texture::PlaceHoldersInfoLeft);
		AddTexture("img/infoPlace22.png"s, (int)Texture::PlaceHoldersInfoCenter);
		AddTexture("img/infoPlace23.png"s, (int)Texture::PlaceHoldersInfoRight);

		AddTexture("img/info_empty.png"s, (int)Texture::PlaceHoldersEmpty);
		AddTexture("img/info_half.png"s, (int)Texture::PlaceHoldersHalf);
		AddTexture("img/info_full.png"s, (int)Texture::PlaceHoldersFull);

		AddTexture("img/border1.png"s, (int)Texture::Border, true);

		AddFont("img/Wander Over Yonder Regular.ttf"s, (int)Font::Message);
	}

	void MainScene::DrawGUI()
	{
		// синхронизируем состо€ние графических элементов с их состо€нием в box2d
		for (b2Body* bd = mainLevel_->GetBodyList(); bd != 0; bd = bd->GetNext()) {
			if (bd->GetUserData().pointer) {
				B2BodyUserDataD* userData = reinterpret_cast<B2BodyUserDataD*>(bd->GetUserData().pointer);

				if (userData->itemType == UiType::Fishka) {
					GUiFishka* f = reinterpret_cast<GUiFishka*>(userData->itemPointer);

					f->SetPosition(camera_->GetPositionInScreen(bd->GetPosition()));
					f->SetRotation(sf::radians(bd->GetAngle()));


				}
			}
		}

		// рисуем элементы из пула в пор€дке приоритета
		Draw();
	}



} // namespace gamecore