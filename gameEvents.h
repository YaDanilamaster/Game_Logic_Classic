#pragma once

// Здесь определены все игровые события.

#include "box2d/box2d.h"
#include "cameraBox2DtoSFML.h"
#include "userInterface.h"

#include "scene.h"
#include "mainScene.h"

#include <iostream>

#include <unordered_map>
#include <memory>
#include <vector>
#include <unordered_set>

namespace gamecore {

	using eventID = size_t;

	struct B2BodyUserDataD;
	class GUiFishka;

	class IEvents
	{
	public:
		virtual bool Step(StepCore& step) = 0;
		virtual ~IEvents() = default;
	};

	enum class JointType
	{
		Mouse = 1,
	};

	// Базовая структура для всех соединений
	struct Joint {
		b2Body* bodyA;
		b2Body* bodyB;
		JointType type;
	};

	// Класс для кеширования событий соединения.
	// Тело В крепится к телу А. Тело А центральная точка крепления.
	// Соединения ожидаются частыми событиями и тратить на них выделение памяти из кучи накладно
	class CashJoints {
	public:
		CashJoints(b2Body* groundBody);
		void AddJoint(Joint&& joint);
		void AddDestroyJoint(b2World* world, b2Joint* joint);
		bool Empty() const;
		void ProcessCash();
	private:
		std::vector<Joint> jointCash_;
		std::vector<std::pair<b2World*, b2Joint*>> destroyCash_;
		b2Body* groundBody_;
		const float maxForce_;
		int joinId_;
		int destroyId_;

		void ProcessJoint();
		void ProcessDestroy();
	};

	enum class EventType
	{
		Spawn = 0,
		Sensor,
		Messadge,
		Show,
	};

	/// <summary>
	/// Класс реализует управление событиями в игре
	/// </summary>
	class EventHandler
	{
	public:
		EventHandler(b2Body* groundBody);
		eventID AddEvent(std::unique_ptr<IEvents>&& event, const EventType type);
		void DeleteEvent(eventID id);

		void AddCashJoint(Joint newJoint);
		void AddCashDestroyJoint(b2World* world, b2Joint* joint);
		void AddUserDataPtr(B2BodyUserDataD* uDPtr);

		void ProcessEvent(StepCore& step);

		~EventHandler();
	private:

		struct MessagePosition {
			sf::Vector2f startPos;
			sf::Vector2f endPos;
			eventID id;
		};

		std::unordered_map<eventID, std::unique_ptr<IEvents>> eventsPool_;
		std::vector<MessagePosition> messagePos_;
		std::vector<B2BodyUserDataD*> b2UserDataPtr_;

		CashJoints jointsCash_;
		size_t curMsgPos_;
		eventID id_ = 0;

	private:
		void ProcessMessagePosition(std::unique_ptr<IEvents>&& event);
	};

	//
	// Ниже определены классы для событий обрабатываемых EventHandler
	// 

	/// <summary>
	/// Массово создает новые фишки
	/// </summary>
	class SpawnFishka final : public IEvents
	{
	public:
		/// <summary>
		/// Вводит в игру новые фишки
		/// </summary>
		/// <param name="items"> - указатели на небходимые фишки</param>
		/// <param name="pos"> - в какой позиции создавать</param>
		/// <param name="durationt"> - за какой промежуток времени (ms) создать указанные фишки</param>
		SpawnFishka(const std::vector<GUiFishka*>& items, const b2Vec2 pos, int durationt);

		bool Step(StepCore& step) override;

		virtual ~SpawnFishka() override;

	private:
		const b2Vec2 pos_;
		std::vector<std::pair<GUiFishka*, b2Vec2>> items_;
		int interval_ = 0;
		int curentDur_ = 0;
		int count_ = 0;
	};


	/// <summary>
	/// Обрабатывает события контакта двух тел
	/// </summary>
	class Sensor final : public b2ContactListener
	{
	public:
		Sensor(EventHandler& eventH);
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
	private:
		EventHandler& eventHandler_;
	};

	/// <summary>
	/// Выводит на экран сообщение с указанной длительностью
	/// </summary>
	class ShowMessage final : public IEvents
	{
	public:
		ShowMessage(GUiText* message, int durationt, int delay = 0);
		bool Step(StepCore& step) override;
		void SetPosition(const sf::Vector2f& startPos, const sf::Vector2f& endPos);

	private:
		GUiText* message_;
		sf::Vector2f moveDuration_;
		int durationt_;
		int curentDur_;
		int delay_;
		int curentDelay_;
		bool isShow_;
	};

	/// <summary>
	/// Событие для демонстрации исходного кода
	/// </summary>
	class ShowCode final : public IEvents
	{
	public:
		ShowCode(std::vector<GUiFishka*> code, const int sleepDurationt);
		bool Step(StepCore& step) override;

	private:
		const std::vector<GUiFishka*> code_;
		const int sleepDurationt_;
		const int emergenceDuration_ = 1500;
		int curentDur_ = 0;
		int curentEmergenceDuration_ = 0;
		bool initStep = true;
	};

} // namespace gamecore