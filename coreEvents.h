#pragma once

/*
������������� ��� Microsoft ������� ������ ��������� �� �������-����� �������� �������
��-�� ���� ��������� �� �������-����� ����� ������ �������������, ��-�� ���� reinterpret_cast<Func>(func)
�� ����� ���������� ��������� � ������������� ������������� ��� � ������������ ���������-�������.

#pragma pointers_to_members( full_generality) - ������� �� ����������� ������������ ������ ������������� ��� ��������� �� �������-����
*/
#pragma pointers_to_members( full_generality)

#include <list>

namespace gamecore {
	/// <summary>
	/// ��������������� ����� ����������� � ���� �������� ��� ������ ����� ��������� ������ ������
	/// </summary>
	/// <typeparam name="...Args">��������� ���������� ��������</typeparam>
	/// <typeparam name="Owner">������ �� ��������� ������</typeparam>
	template <typename Owner, typename... Args>
	class EventSystem {
	public:
		EventSystem() = default;

		void Subscribe(Owner* owner, void (Owner::* func)(Args...)) {
			events_.push_back({ reinterpret_cast<X*>(owner), reinterpret_cast<Func>(func) });
		}

		void Unsubscribe(Owner* owner, void (Owner::* func)(Args...)) {
			X* x = reinterpret_cast<X*>(owner);
			Func fn = reinterpret_cast<Func>(func);

			auto it = events_.begin(), end = events_.end();

			while (it != end)
			{
				if (it->ptr_ == x && it->func_ == fn)
				{
					// ������� �� ������� ���� ��������� � �������� ��������� ������� �������
					// ��� �������� � ������������������ ��������� ��������� �������.
					// ������� ������������ ������� �� ���������, � ���������� ��� ���������
					// ����� ��������� ������� ��� ��� ��������� ������� �������.
					it->isDeleted_ = true;
				}

				++it;
			}
		}

		void operator () (Args... args) {
			auto it = events_.begin(), end = events_.end();

			while (it != end)
			{
				if (it->isDeleted_)
				{
					it = events_.erase(it);
				}
				else {
					Mfn func = reinterpret_cast<Mfn>(it->func_);

					if (it->ptr_ && func) {
						(it->ptr_->*func)(args...);
					}

					++it;
				}
			}
		}

	private:
		struct X {};
		typedef void(X::* Func)();
		typedef void(X::* Mfn)(Args...);

		struct Items {
			X* ptr_;
			Func func_;
			bool isDeleted_ = false;
		};

		std::list<Items> events_;
	};



	class Delegate
	{
	private:

		struct X {};
		typedef void (X::* Func)();

		X* _ptr;
		Func _func;

	public:

		Delegate()
			: _ptr(0), _func(0)
		{
		}

		template <typename Owner, typename... Args>
		Delegate(Owner* owner, void (Owner::* func)(Args...))
		{
			_func = reinterpret_cast<Func>(func);
			_ptr = reinterpret_cast<X*>(owner);
		}

		template <typename Owner>
		Delegate(Owner* owner, void (Owner::* func)())
		{
			_func = reinterpret_cast<Func>(func);
			_ptr = reinterpret_cast<X*>(owner);
		}

		bool operator ! ()
		{
			return (!_ptr || !_func);
		}

		bool operator == (const Delegate& delegate)
		{
			return (_ptr == delegate._ptr && _func == delegate._func);
		}

		bool operator != (const Delegate& delegate)
		{
			return !(*this == delegate);
		}

		void operator () ()
		{
			if (_ptr && _func)
				(_ptr->*_func)();
		}

		template <typename... Args2>
		void operator() (Args2... args)
		{
			typedef void(X::* Mfn)(Args2...);
			Mfn func = reinterpret_cast<Mfn>(_func);

			if (_ptr && func)
				(_ptr->*func)(args...);
		}

	};

} /// namespace core