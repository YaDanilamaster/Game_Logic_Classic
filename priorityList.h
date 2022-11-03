#pragma once
//
// priorityList ���������, ��� �������� ��������� � ������������� �� �������� ���������.
// ��������� ������������ ��� ������ ������ � ����������� ����������,
// �������� � ����������� �� �������� �������� ���� ���������.
// �������� �������� �� ��������� ����������� ��������� ������� ������� ������ �� 2� ����������� � ���������,
// �������� ������ ������� unordered_set/map �� ��������� Vector, List � Deque

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
	/// ��������� � ������ ����� ������� � ��������� �����������. ���������� ������������� �� �����������
	/// ��� �������� �������� �� ������ ���������� ����� ��� �����������
	/// </summary>
	/// <param name="item"> - ��������� �� ����� �������</param>
	/// <param name="priority"> - ���������</param>
	/// <param name="canBeDeleted"> - ������� ����� ���� ������.
	/// ��� �� ��������� ��������� �� �������� �������������� ���, ��� ���������� ������ ������.</param>
	void Add(T* item, const int priority, const bool canBeDeleted = false);
	void Delete(T* item);

	// ���������� �� ������ �.�. ��� 1 ��� ������� ������ ������� ���� � 78 ���!
	// �������� � ����������, 60 ��� � ������� ����������. �������� � ���������� �����, ��� � 10-20 ���.

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

	std::vector<PriorityGroup> priorityItems_;								// ��������� ��� ������� �������� �� ���������
	std::unordered_map<int, std::vector<std::unique_ptr<T>>*> allPriority_;	// ������ ��� ������������� ����������, ����� ������ ����������� ����� ����
	std::unordered_map<T*, int> itemToPriority_;							// �������� �������� �������� ������ �� ��������� � ����������� � ������� ���������
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
		// ������������ ���������� ���, ����� ��� ���������� ����� ������ ������������� ����������.
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
