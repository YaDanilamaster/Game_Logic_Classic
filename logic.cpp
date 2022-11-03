#include "logic.h"

#include <iostream>

namespace gamecore {

	Logic::Logic(const int code_color_count, const int color_count_max)
		: color_count_(color_count_max),
		code_count_(code_color_count)
	{
	}

	const std::vector<int>& Logic::CreateNewCode(const bool unique)
	{
		//  оличество цветов должно быть больше количества значений.
		// ƒопускаетс€ иное только если цвета не уникальные.

		code_.clear();
		color_position_.clear();

		code_.resize(code_count_, -1);
		color_position_.resize(color_count_, -1);

		std::mt19937 generator;
		// вектор из сырой последовательности наполненной дубликатами
		const std::vector<int> raw_colors = GenerateInt(generator, color_count_ - 1);

		int i = 0, j = 0;
		while (i < code_count_ && j < raw_colors.size()) {

			if (code_[i] == -1 && (unique && color_position_[raw_colors[j]] == -1)) {
				color_position_[raw_colors[j]] = i;
				code_[i++] = raw_colors[j++];
			}
			else {
				if (unique && (color_position_[raw_colors[j]] != -1)) {
					j++;
				}
				else {
					color_position_[raw_colors[j]] = i;
					code_[i++] = raw_colors[j++];
				}
			}
		}

		if (j == raw_colors.size()) throw std::logic_error::logic_error("Wrong random generator logic.");

		return code_;
	}

	const std::vector<int>& Logic::GetClorPosition() const
	{
		return color_position_;
	}

	const std::vector<int>& Logic::GetCode() const
	{
		return code_;
	}


	std::vector<Compliance> Logic::GetCodeBreake(const std::vector<int>& code, const bool matching_positions)
	{
		std::vector<Compliance> result(code_count_, Compliance::None);

		for (int i = 0; i < code_count_; i++) {
			int color = code[i];
			int position = color_position_[color];

			if (position == i) {
				result[i] = Compliance::Full;
			}
			else if (position != -1) {
				if (matching_positions) {
					result[i] = Compliance::Partial;
				}
				else {
					result[i] = Compliance::Partial;
				}
			}
			else {
				result[i] = Compliance::None;
			}
		}

		if (!matching_positions) {
			std::sort(result.begin(), result.end(), [](const Compliance lhs, const Compliance rhs)
				{
					return static_cast<int>(lhs) < static_cast<int>(rhs);
				});
		}

		return result;
	}

	int Logic::GetColorCount()
	{
		return color_count_;
	}

	int Logic::GetNumCount()
	{
		return code_count_;
	}

	std::vector<int> Logic::GenerateInt(std::mt19937& generator, const int color_count, const bool unique_color)
	{
		int max = color_count * 10;
		std::vector<int> nums(max);

		const Clock::time_point start_time_ = Clock::now();
		auto msec = std::chrono::duration_cast<std::chrono::microseconds>(start_time_.time_since_epoch());

		generator.seed(msec.count());

		for (int i = 0; i < max; ++i) {
			nums[i] = std::uniform_int_distribution<int>(0, color_count)(generator);
		}
		return nums;
	}
} //namespace gamecore