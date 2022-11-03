#include "menuScene.h"


namespace gamecore {

	MenuScene::MenuScene(sf::RenderWindow* window)
		: Scene(window)
	{

	}

	void MenuScene::MainLoop()
	{
		sf::Vector2f pos;
		while (window_->isOpen() && exitCode_ == ExitSceneCode::None)
		{
			sf::Event event;
			while (window_->pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
					window_->close();
					exitCode_ = ExitSceneCode::CloseWindow;
					break;
				case sf::Event::MouseMoved:
					pos = window_->mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
					eventsMouseMoved_(sf::Event::MouseMoveEvent{ (int)pos.x, (int)pos.y });
					break;
				case sf::Event::MouseButtonPressed:
					pos = window_->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
					eventsMouseDown_({ event.mouseButton.button, (int)pos.x, (int)pos.y }, this);
					break;
				}
			}

			window_->clear();

			Draw();

			window_->display();

		}
	}

	void MenuScene::Initialization()
	{
		sf::Vector2f size = { (float)window_->getSize().x, (float)window_->getSize().y };

		ruleView_.reset(sf::FloatRect(centerRule_, size));
		settingsView_.reset(sf::FloatRect(centerSettings_, size));

		AddTexture("img/back_ground.jpg"s, (int)Texture::BackGround);
		Scene::CreateGUInterface<GUiSprite>(0, false, GetTexture((int)Texture::BackGround));
		
		sf::Vector2f center = size / 2.f;

		AddFont("img/Wander Over Yonder Regular.ttf"s, (int)Font::Message);

		{
			auto* msg = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::NewGame);

			msg->SetString(L"����� ����");
			msg->SetCharacterSize(70);
			msg->SetFillColor(ColorStyle.base);
			sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
			msg->SetPosition({ center.x - msgRect.width / 2.f, center.y - 200 });
		}
		{
			auto* msg = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Settings);

			msg->SetString(L"���������");
			msg->SetCharacterSize(70);
			msg->SetFillColor(ColorStyle.base);
			sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
			msg->SetPosition({ center.x - msgRect.width / 2u, center.y - 100});
		}
		{
			auto* msg = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Rule);

			msg->SetString(L"������� ����");
			msg->SetCharacterSize(70);
			msg->SetFillColor(ColorStyle.base);
			sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
			msg->SetPosition({ center.x - msgRect.width / 2u, center.y });
		}
		// ��������� �������� ������
		sf::Vector2u windowSize = window_->getSize();

		{
			auto* rule = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown, GetFont((int)Font::Message), (int)MenuItem::GoMenu);

			rule->SetString(ruleText);
			rule->SetCharacterSize(37);
			rule->SetFillColor(ColorStyle.base);

			sf::FloatRect msgRect = rule->GetText().getGlobalBounds();
			rule->SetPosition(centerRule_ +sf::Vector2f({ 50, 20 }));
		}

		// ��������� �������� ��������
		center = settingsView_.getCenter();
		{
			auto* settingText = Scene::CreateGUInterface<GUiText>(1, false, GetFont((int)Font::Message), -1);

			settingText->SetString(L"�������� ������� ���������");
			settingText->SetCharacterSize(70);
			settingText->SetFillColor(ColorStyle.highlighting);

			sf::FloatRect msgRect = settingText->GetText().getGlobalBounds();
			settingText->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - msgRect.width / 2, center.y - 350 }))));
		}
		{
			auto* msg = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Easy);

			msg->SetString(L"�����");
			msg->SetCharacterSize(60);
			msg->SetFillColor(ColorStyle.base);
			sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
			msg->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - 400, center.y - 200 }))));
		}
		{
			auto* msg = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Normal);

			msg->SetString(L"���������");
			msg->SetCharacterSize(60);
			msg->SetFillColor(ColorStyle.base);
			sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
			msg->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - 400, center.y - 100}))));
		}
		{
			auto* msg = CreateGUInterface<GUiLabel>
				(1, false, Events::MouseDown | Events::MouseMoved, GetFont((int)Font::Message), (int)MenuItem::Hard);

			msg->SetString(L"������");
			msg->SetCharacterSize(60);
			msg->SetFillColor(ColorStyle.base);
			sf::FloatRect msgRect = msg->GetText().getGlobalBounds();
			msg->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - 400, center.y }))));
		}

		// �������� ������� ���������
		{
			auto* settingText = Scene::CreateGUInterface<GUiText>(1, false, GetFont((int)Font::Message), -1);
			settingText->SetString(LR"(������� ���������� ������������� ������� � ����,
����� �� �����������.)");
			settingText->SetCharacterSize(30);
			settingText->SetFillColor(ColorStyle.base);

			sf::FloatRect msgRect = settingText->GetText().getGlobalBounds();
			settingText->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - 50, center.y - 200 }))));
		}
		{
			auto* settingText = Scene::CreateGUInterface<GUiText>(1, false, GetFont((int)Font::Message), -1);
			settingText->SetString(LR"(���������� �� ��������� � �������� � ����,
����� �� �����������.)");
			settingText->SetCharacterSize(30);
			settingText->SetFillColor(ColorStyle.base);

			sf::FloatRect msgRect = settingText->GetText().getGlobalBounds();
			settingText->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - 50, center.y - 100 }))));
		}
		{
			auto* settingText = Scene::CreateGUInterface<GUiText>(1, false, GetFont((int)Font::Message), -1);
			settingText->SetString(LR"(���������� �� ��������� � �������� � ����,
����� � ���� ����� �����������.)");
			settingText->SetCharacterSize(30);
			settingText->SetFillColor(ColorStyle.base);

			sf::FloatRect msgRect = settingText->GetText().getGlobalBounds();
			settingText->SetPosition(sf::Vector2f(
				window_->mapCoordsToPixel(sf::Vector2f({ center.x - 50, center.y }))));
		}
	}

	MenuScene::Difficulty MenuScene::GetDifficulty()
	{
		return difficulty_;
	}


	void MenuScene::MousePushCallBack(const int labelId)
	{
		switch (static_cast<MenuItem>(labelId))
		{
		case MenuItem::NewGame:
			exitCode_ = ExitSceneCode::NewScene;
			break;
		case MenuItem::Rule:
			window_->setView(ruleView_);
			break;
		case MenuItem::Settings:
			window_->setView(settingsView_);
			break;
		case MenuItem::GoMenu:
			window_->setView(window_->getDefaultView());
			break;
		case MenuItem::Easy:
			window_->setView(window_->getDefaultView());
			difficulty_ = Difficulty::Easy;
			break;
		case MenuItem::Normal:
			window_->setView(window_->getDefaultView());
			difficulty_ = Difficulty::Normal;
			break;
		case MenuItem::Hard:
			window_->setView(window_->getDefaultView());
			difficulty_ = Difficulty::Hard;
			break;

		default:
			break;
		}
	}

} // namespace gamecore