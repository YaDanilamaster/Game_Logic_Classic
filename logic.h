#pragma once

#include <vector>
#include <random>
#include <chrono>
#include <exception>
#include <algorithm>

namespace gamecore {

	enum class Compliance
	{
		Full = 0,
		Partial,
		None,
	};

	class Logic {
		using Clock = std::chrono::steady_clock;
	public:
		Logic(const int code_color_count, const int color_count_max);

		// Получить новый код
		const std::vector<int>& CreateNewCode(const bool unique = true);

		// Получить позиции цветов
		const std::vector<int>& GetClorPosition() const;

		// Получить исходный цветовой код
		const std::vector<int>& GetCode() const;

		/// <summary>
		/// Получить информацию по совпадениям
		/// </summary>
		/// <param name="code"> - Код для проверки на совпадения</param>
		/// <param name="matching_positions"> - Должны ли совпадать соответствующие позиции</param>
		/// <returns></returns>
		std::vector<Compliance> GetCodeBreake(const std::vector<int>& code, const bool matching_positions = false);

		// Возвращает количество цветов
		int GetColorCount();

		// Возвращает количество позиций в коде
		int GetNumCount();

	private:
		std::vector<int> code_;					// цветовой код
		std::vector<int> color_position_;		// цвета на позициях
		const int color_count_;
		const int code_count_;

		/// <summary>
		/// Генерирует случайную числовую последовательность
		/// </summary>
		/// <param name="generator"> - std::mt19937</param>
		/// <param name="color_count"> - Количество необходимых цветов</param>
		/// <param name="unique_color"> - Должны ли цвета быть уникальными</param>
		/// <returns></returns>
		std::vector<int> GenerateInt(std::mt19937& generator, const int color_count, const bool unique_color = true);

	};

} // namespace gamecore