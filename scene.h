#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "cameraBox2DtoSFML.h"
#include "priorityList.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <random>


namespace gamecore {

	namespace Events {
		enum
		{
			None = 0,
			MouseMoved = 1u << 0,
			MouseDown = 1u << 1,
		};
	}

	enum class ExitSceneCode
	{
		None = 0,
		CloseWindow,
		NewScene,
		Restart,
	};

	static const struct
	{
		const sf::Color base = { 255, 255, 255, 255 };
		const sf::Color highlighting = { 255, 191, 0, 255 };
		const sf::Color blocked = { 170, 170, 170, 255 };
	} ColorStyle;

	class EventHandler;
	class GUInterface;

	class Scene
	{
	public:
		Scene(sf::RenderWindow* window);

		virtual void MainLoop() = 0;
		virtual void Draw();
		virtual void MousePushCallBack(const int id) = 0;

		void AddTexture(const std::filesystem::path& file, const int texture, const bool isRepeated = false);
		void AddFont(const std::filesystem::path& file, const int font);

		sf::Texture* GetTexture(const int id) const;
		sf::Font* GetFont(const int id) const;

		/// <summary>
		/// ���������� ��� � ������� ����� ���������
		/// </summary>
		/// <returns></returns>
		ExitSceneCode GetExitCode() const;

		/// <summary>
		/// ������� ����������� ������� � ���������� ����������� � ��������� ��� � ������� �� ���������
		/// </summary>
		/// <typeparam name="T"> ��� ������������ ��������</typeparam>
		/// <typeparam name="...Args"> ����� ����������</typeparam>
		/// <param name="priority"> - ������� �� ���������. ��� ���� ���������, ��� ������ ������� ����� ���������</param>
		/// <param name="canBeDeleted"> - ���� ������� �� ����� ��������� ����� �� ���� ����� �������� ������ ������</param>
		/// <returns></returns>
		template<typename T, typename ...Args>
		T* CreateGUInterface(const int priority, const bool canBeDeleted, Args&& ...);

		virtual ~Scene();
	private:
		/// <summary>
		/// ��������� �� ����� ����������� ������� � ���������� ��������� �� ���������� ������
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="file"></param>
		/// <returns></returns>
		template <typename T>
		T* LoadFromFile(const std::filesystem::path& file);

	protected:
		sf::RenderWindow* window_;
		std::unordered_map<int, std::unique_ptr<sf::Texture>> texturePool_;
		std::unordered_map<int, std::unique_ptr<sf::Font>> fontPool_;

		// �������� ����������
		PriorityList<sf::Drawable> userInterfaceDraw_;						// ��������� � ����������� �� ��������, ������� ���������� ������������

		Box2DCamera* camera_ = nullptr;
		EventHandler* eventsGame_ = nullptr;

		ExitSceneCode exitCode_ = ExitSceneCode::None;

	public:		// �������� ���������
		const float WORLDSTEEP = 1 / 60.f;	// ���������� ������ � ������� ��� ��������� ����������� ����
		const float FRAMERATELIMIT = 60;	// 
		const int COLOR_COUNT = 5;			// ���������� ������ � ����
		const int COLOR_COUNT_MAX = 8;		// ����� ���������� ������ � ����
		const bool COLOR_UNIQUE = true;		// ������ �� ����� ���� �����������. �� ���������� ����� ����������� ��������� ������� ����

	};

	template<typename T, typename ...Args>
	inline T* Scene::CreateGUInterface(const int priority, const bool canBeDeleted, Args&& ...arg)
	{
		T* ptr = new T(arg...);
		userInterfaceDraw_.Add(ptr, priority, canBeDeleted);
		return ptr;
	}

	template <typename T>
	T* Scene::LoadFromFile(const std::filesystem::path& file) {
		T* txt = new T;
		if (txt->loadFromFile(file))
		{
			txt->setSmooth(true);
			return txt;
		}

		assert(true, "Error load file: "s + file);
	}

	struct StepCore
	{
		b2World* world;								// ������� ���
		Box2DCamera* camera;						// ������� ������
		Scene* scene;								// ������� ������� ����
		EventHandler* eventHandler;					// ������� ���������� �������

		size_t stepCount;							// ���������� ����� � ������� �������
		int prevDuration;							// ����� ����������� ����, ms
		int curentDuration;							// ����� �������� ����, ms
	};

	int RandomGenerate(std::mt19937& generator, int min, int max);


} // namespace gamecore