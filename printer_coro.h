#include <experimental/coroutine>
#include <assert.h>
#include <iostream>
#include <string>

namespace Coroutines {
	class Printer
	{
	private:
		class Promise
		{
		private:
			using CoroHandle = std::experimental::coroutine_handle<Promise>;
			using suspend_always = std::experimental::suspend_always;

		public:
			auto get_return_object() { return CoroHandle::from_promise(*this); }
			auto initial_suspend() { return suspend_always{}; }
			auto final_suspend() { return suspend_always{}; }
			void return_void() { }
			void unhandled_exception() { std::terminate(); }
		};
		using CoroHandle = std::experimental::coroutine_handle<Promise>;

	public:
		using promise_type = Promise;

		Printer(CoroHandle&& handle) : coro_handle(std::move(handle)) { assert(coro_handle); }
		Printer(Printer&&) = default;
		Printer(const Printer&) = delete;
		~Printer() { coro_handle.destroy(); }

		Printer& operator=(Printer&&) = default;
		Printer& operator=(const Printer&) = delete;

		bool resume() {
			if (!coro_handle.done()) {
				coro_handle.resume();
			}
			return !coro_handle.done();
		}

	private:
		CoroHandle coro_handle;
	};

	auto print_line(std::string message) -> Printer {
		//initial suspend happens here

		//message is printed when the coroutine is resumed
		std::cout << message << std::endl;
		
		//after co_return, the coroutine reaches the final suspend point
		//which means it is done but not finished
		co_return;
	}
}