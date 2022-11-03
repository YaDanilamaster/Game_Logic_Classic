#include "scene.h"

namespace gamecore {

	gamecore::Scene::Scene(sf::RenderWindow* window)
		: window_(window)
	{

	}

	void Scene::Draw()
	{
		for (auto&[priority, items] : userInterfaceDraw_)
		{
			for (std::unique_ptr<sf::Drawable>& draw : *items.get())
			{
				window_->draw(*draw.get());
			}
		}
	}

	void Scene::AddTexture(const std::filesystem::path& file, const int texture, const bool isRepeated)
	{
		std::unique_ptr<sf::Texture> newTexture = std::unique_ptr<sf::Texture>(LoadFromFile<sf::Texture>(file));
		newTexture->setRepeated(isRepeated);
		texturePool_.insert({ texture, std::move(newTexture) });
	}

	void Scene::AddFont(const std::filesystem::path& file, const int font)
	{
		std::unique_ptr<sf::Font> newFont = std::unique_ptr<sf::Font>(LoadFromFile<sf::Font>(file));
		fontPool_.insert({ font, std::move(newFont) });
	}

	sf::Texture* Scene::GetTexture(const int id) const
	{
		return texturePool_.find(id)->second.get();
	}

	sf::Font* Scene::GetFont(const int id) const
	{
		return fontPool_.find(id)->second.get();
	}

	ExitSceneCode Scene::GetExitCode() const
	{
		return exitCode_;
	}

	Scene::~Scene()
	{
		if (camera_) {
			delete camera_;
		}
		if (eventsGame_) {
			delete eventsGame_;
		}
	}


	int RandomGenerate(std::mt19937& generator, int min, int max) {
		generator.seed(std::chrono::steady_clock::now().time_since_epoch().count());
		return std::uniform_int_distribution(min, max)(generator);
	}


} // namespace gamecore