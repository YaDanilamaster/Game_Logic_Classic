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

		// �������� ����� ���
		const std::vector<int>& CreateNewCode(const bool unique = true);

		// �������� ������� ������
		const std::vector<int>& GetClorPosition() const;

		// �������� �������� �������� ���
		const std::vector<int>& GetCode() const;

		/// <summary>
		/// �������� ���������� �� �����������
		/// </summary>
		/// <param name="code"> - ��� ��� �������� �� ����������</param>
		/// <param name="matching_positions"> - ������ �� ��������� ��������������� �������</param>
		/// <returns></returns>
		std::vector<Compliance> GetCodeBreake(const std::vector<int>& code, const bool matching_positions = false);

		// ���������� ���������� ������
		int GetColorCount();

		// ���������� ���������� ������� � ����
		int GetNumCount();

	private:
		std::vector<int> code_;					// �������� ���
		std::vector<int> color_position_;		// ����� �� ��������
		const int color_count_;
		const int code_count_;

		/// <summary>
		/// ���������� ��������� �������� ������������������
		/// </summary>
		/// <param name="generator"> - std::mt19937</param>
		/// <param name="color_count"> - ���������� ����������� ������</param>
		/// <param name="unique_color"> - ������ �� ����� ���� �����������</param>
		/// <returns></returns>
		std::vector<int> GenerateInt(std::mt19937& generator, const int color_count, const bool unique_color = true);

	};

} // namespace gamecore