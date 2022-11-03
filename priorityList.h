#pragma once
//
// priorityList контейнер, для хранения элементов в упорядоченном по приориту состоянии.
// Контейнер предназначен для работы только с уникальными элементами,
// построен с приоритетом на скорость перебора всех элементов.
// Скорость итераций по коллекции практически идентична прямому вектору вплоть до 2к приоритетов в коллекции,
// скорость записи быстрее unordered_set/map но медленнее Vector, List и Deque

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <execution>

template <typename T>
class PriorityList
{
public:
	PriorityList() = default;
	/// <summary>
	/// Добавялет в список новый элемент с указанным приоритетом. Приоритеты отсортированы по возрастанию
	/// При удалении элемента из списка происходит вызов его деструктора
	/// </summary>
	/// <param name="item"> - указатель на новый элемент</param>
	/// <param name="priority"> - приоритет</param>
	/// <param name="canBeDeleted"> - элемент может быть удален.
	/// Для не удаляемых элементов не строится дополнительный кеш, что потребляет меньше памяти.</param>
	void Add(T* item, const int priority, const bool canBeDeleted = false);
	void Delete(T* item);

	// переделать на вектор т.к. при 1 млн записей вектор быстрее мапы в 78 раз!
	// итерации в приоритете, 60 раз в секунду происходят. удаление и добавление редки, раз в 10-20 сек.

	const auto begin() const;
	const auto end() const;
	auto begin();
	auto end();

private:
	struct PriorityGroup
	{
		int priority;
		std::unique_ptr<std::vector<std::unique_ptr<T>>> items;
	};

	std::vector<PriorityGroup> priorityItems_;								// контейнер для быстрых итераций по коллекции
	std::unordered_map<int, std::vector<std::unique_ptr<T>>*> allPriority_;	// храним все установленные приоритеты, чтобы быстро пермещаться между ними
	std::unordered_map<T*, int> itemToPriority_;							// удялемые элементы содержат ссылку на контейнер с приоритетом в котором находятся
};

template<typename T>
inline void PriorityList<T>::Add(T* item, const int priority, const bool canBeDeleted)
{
	if (canBeDeleted)
	{
		itemToPriority_.insert({ item, priority });
	}

	auto it = allPriority_.find(priority);
	if (it != allPriority_.end()) {
		it->second->push_back(std::unique_ptr<T>(item));
	}
	else
	{
		// добавляемого приоритета нет, после его добавления нужно заново отсортировать приоритеты.
		std::unique_ptr<T> newItem(item);
		auto newVector = std::make_unique<std::vector<std::unique_ptr<T>>>();
		newVector->push_back(std::move(newItem));
		std::unique_ptr<std::vector<std::unique_ptr<T>>> itemPtr(std::move(newVector));

		allPriority_[priority] = itemPtr.get();
		priorityItems_.push_back({ priority, std::move(itemPtr) });

		std::sort(std::execution::par, priorityItems_.begin(), priorityItems_.end(),
			[](const PriorityGroup& lhs, const PriorityGroup& rhs)
			{
				return lhs.priority < rhs.priority;
			}
		);
	}
}

template<typename T>
inline void PriorityList<T>::Delete(T* item)
{
	auto it = itemToPriority_.find(item);
	if (it != itemToPriority_.end())
	{
		auto itPriority = allPriority_.find(it->second);
		std::vector<std::unique_ptr<T>>* items = itPriority->second;

		auto fnd = std::find_if(std::execution::par, items->begin(), items->end(),
			[&item](const std::unique_ptr<T>& other)
			{
				return other.get() == item;
			});

		items->erase(fnd);

		if (items->empty())
		{
			auto itItems = std::find_if(std::execution::par, priorityItems_.begin(), priorityItems_.end(),
				[&itPriority](const PriorityGroup& value) {
					return value.priority == itPriority->first;
				}
			);

			allPriority_.erase(itPriority);
			priorityItems_.erase(itItems);
		}

		itemToPriority_.erase(it);
	}
}

template<typename T>
inline const auto PriorityList<T>::begin() const
{
	return priorityItems_.begin();
}

template<typename T>
inline const auto PriorityList<T>::end() const
{
	return priorityItems_.end();
}

template<typename T>
inline auto PriorityList<T>::begin()
{
	return priorityItems_.begin();
}

template<typename T>
inline auto PriorityList<T>::end()
{
	return priorityItems_.end();
}
