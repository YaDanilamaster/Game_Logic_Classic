#include "gameEvents.h"

namespace gamecore {

	// ѕредустановки дл€ начальных координат сообщений
	EventHandler::EventHandler(b2Body* groundBody)
		: jointsCash_(groundBody)
		, messagePos_({
			{{40, 90}, {40, 20}, 0},
			{{40, 90}, {40, 20}, 0},
			{{40, 90}, {40, 20}, 0},
			{{40, 90}, {40, 20}, 0},
			{{40, 90}, {40, 20}, 0},
			})
			, curMsgPos_(0)
	{
	}

	eventID EventHandler::AddEvent(std::unique_ptr<IEvents>&& event, const EventType type)
	{
		if (type == EventType::Messadge) {
			ProcessMessagePosition(std::move(event));
		}
		else {
			eventsPool_[++id_] = std::move(event);
		}
		return id_;
	}

	void EventHandler::ProcessMessagePosition(std::unique_ptr<IEvents>&& event) {
		//if (curMsgPos_ == messagePos_.size()) {
		//	curMsgPos_ = 0;
		//}

		//if (messagePos_[curMsgPos_].id != 0) {
		//	DeleteEvent(messagePos_[curMsgPos_].id);
		//}

		//messagePos_[curMsgPos_].id = ++id_;

		curMsgPos_ = 0;
		ShowMessage* msg = static_cast<ShowMessage*>(event.get());
		msg->SetPosition(messagePos_[curMsgPos_].startPos, messagePos_[curMsgPos_].endPos);

		eventsPool_[++id_] = std::move(event);
		++curMsgPos_;
	}

	void EventHandler::AddCashJoint(Joint newJoint)
	{
		jointsCash_.AddJoint(std::move(newJoint));
	}

	void EventHandler::AddCashDestroyJoint(b2World* world, b2Joint* joint)
	{
		jointsCash_.AddDestroyJoint(world, joint);
	}

	void EventHandler::AddUserDataPtr(B2BodyUserDataD* uDPtr)
	{
		b2UserDataPtr_.push_back(uDPtr);
	}

	void EventHandler::DeleteEvent(eventID id)
	{
		eventsPool_.erase(id);
	}

	void EventHandler::ProcessEvent(StepCore& step)
	{
		// —начала обрабатываем событи€ соединени€
		if (!step.eventHandler->jointsCash_.Empty()) {
			step.eventHandler->jointsCash_.ProcessCash();
		}

		// ѕотом обрабатываем событи€ €дра
		auto it = step.eventHandler->eventsPool_.begin();
		auto end = step.eventHandler->eventsPool_.end();

		while (it != end) {
			auto& [id, item] = *it;
			bool isEnd = item->Step(step);

			if (!isEnd) {
				++it;
			}
			else {
				it = step.eventHandler->eventsPool_.erase(it);
			}
		}
	}

	EventHandler::~EventHandler()
	{
		for (auto i : b2UserDataPtr_)
		{
			delete i;
		}
	}

	SpawnFishka::SpawnFishka(const std::vector<GUiFishka*>& items, const b2Vec2 pos, int durationt)
		: pos_(pos)
		, curentDur_(0)
	{
		interval_ = durationt / items.size();
		count_ = items.size();
		items_.resize(count_);

		std::mt19937 generator;
		for (size_t i = 0; i < count_; i++) {
			int rndx = gamecore::RandomGenerate(generator, -200, 200);
			int rndy = gamecore::RandomGenerate(generator, -200, 200);

			items_[i] = { items[i], {(float)rndx, (float)rndy} };
		}
	}

	bool SpawnFishka::Step(StepCore& step)
	{
		curentDur_ += step.curentDuration;
		if (curentDur_ >= interval_) {
			--count_;
			MainScene* scene = static_cast<MainScene*>(step.scene);
			b2Body* f = scene->CreateBodyFishka(pos_.x, pos_.y);
			auto& it = items_[count_];

			// ѕрисваиваем UserData указатель на фишку.
			B2BodyUserDataD* userData = new B2BodyUserDataD;
			step.eventHandler->AddUserDataPtr(userData);

			userData->itemType = UiType::Fishka;
			userData->isDrawable = true;
			userData->itemPointer = it.first;

			f->ApplyLinearImpulseToCenter({ it.second.x, it.second.y }, true);
			f->GetUserData().pointer = reinterpret_cast<uintptr_t>(userData);
			curentDur_ = 0;
		}

		return count_ == 0;
	}

	SpawnFishka::~SpawnFishka()
	{}

	Sensor::Sensor(EventHandler& eventH)
		: eventHandler_(eventH)
	{
	}

	void Sensor::BeginContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		if (fixtureA->IsSensor() || fixtureB->IsSensor()) {

			b2Body* sensBoby;
			b2Body* dinamicBoby;
			if (fixtureA->IsSensor()) {
				sensBoby = fixtureA->GetBody();
				dinamicBoby = fixtureB->GetBody();
			}
			else {
				sensBoby = fixtureB->GetBody();
				dinamicBoby = fixtureA->GetBody();
			}

			B2BodyUserDataD* sensUserData = reinterpret_cast<B2BodyUserDataD*>(sensBoby->GetUserData().pointer);

			if (sensUserData->cash == 0) {
				// “.к. измен€ть мир внутри обратных вызовов запрещено - кешируем параметры соединений.
				sensUserData->cash = 1;
				// сохран€ем ссылку на фишку, которую будет удерживать сенсор
				sensUserData->itemPointer = (reinterpret_cast<B2BodyUserDataD*>(dinamicBoby->GetUserData().pointer))->itemPointer;
				eventHandler_.AddCashJoint({ sensBoby, dinamicBoby, JointType::Mouse });
			}

		}
	}

	void Sensor::EndContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		if (fixtureA->IsSensor() || fixtureB->IsSensor()) {

			b2Body* sensBoby;
			b2Body* dinamicBoby;
			if (fixtureA->IsSensor()) {
				sensBoby = fixtureA->GetBody();
				dinamicBoby = fixtureB->GetBody();
			}
			else {
				sensBoby = fixtureB->GetBody();
				dinamicBoby = fixtureA->GetBody();
			}

			B2BodyUserDataD* sensUserData = reinterpret_cast<B2BodyUserDataD*>(sensBoby->GetUserData().pointer);

			if (sensUserData->cash == 1 && sensUserData->itemPointer == (reinterpret_cast<B2BodyUserDataD*>(dinamicBoby->GetUserData().pointer))->itemPointer) {
				sensUserData->cash = 0;
				eventHandler_.AddCashDestroyJoint(sensBoby->GetWorld(), sensBoby->GetJointList()->joint);

				// ≈сли произошло выдавливание фишки тогда надо создать новую св€зь между сенсором и выдавившей фишкой
				b2ContactEdge* nextContact = sensBoby->GetContactList()->next;
				// нова€ св€зь не нужна, если фишка уже прив€зана к одному из сенсоров
				if (nextContact) {
					dinamicBoby = sensBoby->GetContactList()->other;

					sensUserData->cash = 1;
					// сохран€ем ссылку на фишку, которую будет удерживать сенсор
					sensUserData->itemPointer = (reinterpret_cast<B2BodyUserDataD*>(dinamicBoby->GetUserData().pointer))->itemPointer;

					eventHandler_.AddCashJoint({ sensBoby, dinamicBoby, JointType::Mouse });
				}
			}
		}
	}

	CashJoints::CashJoints(b2Body* groundBody)
		: jointCash_(10)
		, destroyCash_(10)
		, groundBody_(groundBody)
		, maxForce_(1000.f)
		, joinId_(0)
		, destroyId_(0)
	{
	}

	void CashJoints::AddJoint(Joint&& joint)
	{
		jointCash_[joinId_++] = std::move(joint);
		if (joinId_ == jointCash_.size()) {
			jointCash_.resize(jointCash_.size() + 10);
		}
	}

	void CashJoints::AddDestroyJoint(b2World* world, b2Joint* joint)
	{
		destroyCash_[destroyId_++] = { world, joint };
		if (destroyId_ == destroyCash_.size()) {
			destroyCash_.resize(destroyCash_.size() + 10);
		}
	}

	bool CashJoints::Empty() const
	{
		return joinId_ == 0 && destroyId_ == 0;
	}

	void CashJoints::ProcessCash()
	{
		// —начала обрабатываем соединени€, потом отсоединени€.
		ProcessJoint();
		ProcessDestroy();
	}

	void CashJoints::ProcessJoint()
	{
		while (joinId_ > 0) {
			Joint& item = jointCash_[--joinId_];
			b2World* world = item.bodyB->GetWorld();

			switch (item.type)
			{
			case JointType::Mouse:
				float frequencyHz = 5.0f;
				float dampingRatio = 0.7f;

				b2MouseJointDef mjd;

				mjd.bodyA = item.bodyA;
				mjd.bodyB = item.bodyB;
				mjd.collideConnected = true;
				mjd.target = item.bodyB->GetWorldCenter();
				mjd.maxForce = 5000.0f;
				b2LinearStiffness(mjd.stiffness, mjd.damping, frequencyHz, dampingRatio, mjd.bodyA, mjd.bodyB);

				b2MouseJoint* newMjd = (b2MouseJoint*)world->CreateJoint(&mjd);
				newMjd->SetTarget(item.bodyA->GetWorldCenter());

				break;
			}
		}
	}

	void CashJoints::ProcessDestroy()
	{
		while (destroyId_ > 0) {
			auto [world, joint] = destroyCash_[--destroyId_];
			world->DestroyJoint(joint);
		}
	}

	ShowMessage::ShowMessage(GUiText* message, int durationt, int delay)
		: message_(message)
		, durationt_(durationt)
		, isShow_(false)
		, delay_(delay)
		, curentDelay_(0)
		,curentDur_(0)
	{
	}

	bool gamecore::ShowMessage::Step(StepCore& step)
	{
		if (!isShow_)
		{
			if (curentDelay_ < delay_)
			{
				curentDelay_ += step.curentDuration;
				return false;
			}

			isShow_ = true;
			MainScene* scene = static_cast<MainScene*>(step.scene);
			scene->userInterfaceDraw_.Add(message_, 5, true);
		}

		if (curentDur_ >= durationt_)
		{
			MainScene* scene = static_cast<MainScene*>(step.scene);
			scene->userInterfaceDraw_.Delete(message_);
			return true;
		}
		else {
			message_->GetText().move(moveDuration_ * (float)step.curentDuration);
			curentDur_ += step.curentDuration;
			return false;
		}
	}

	void ShowMessage::SetPosition(const sf::Vector2f& startPos, const sf::Vector2f& endPos) 
	{
		message_->SetPosition(startPos);
		moveDuration_ = (endPos - startPos) / (float)durationt_;
	}


	ShowCode::ShowCode(std::vector<GUiFishka*> code, const int sleepDurationt)
		: code_(std::move(code))
		, sleepDurationt_(sleepDurationt)
	{

	}

	bool ShowCode::Step(StepCore& step) {
		if (curentDur_ < sleepDurationt_) {
			curentDur_ += step.curentDuration;
			if (initStep) {
				MainScene* scene = static_cast<MainScene*>(step.scene);
				for (GUiFishka* f : code_) {
					f->GetSprite().setColor({ 255, 255, 255, 0 });
				}
				initStep = false;
			}
		}
		else if (curentEmergenceDuration_ < emergenceDuration_) {
			curentEmergenceDuration_ += step.curentDuration;
			int alfa = (curentEmergenceDuration_ / (float)emergenceDuration_) * 255.f;
			alfa = alfa > 250 ? 255 : alfa;

			if (alfa < 255) {
				for (size_t i = 0; i < code_.size(); i++) {
					sf::Sprite& sprite = code_[i]->GetSprite();
					const sf::FloatRect& rect = sprite.getGlobalBounds();

					sprite.setColor({ 255, 255, 255, (uint8)alfa });
					sprite.setPosition({ 150 + i * (rect.width + 20), 80 });
				}
			}
		}
		return false;
	}
} //namespace gamecore