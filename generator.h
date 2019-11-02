#include <experimental/coroutine>
#include <assert.h>

namespace Coroutines {
	template <typename T>
	class Generator
	{
	private:
		static_assert(std::is_default_constructible_v<T>, "Generator<T> requires T to be default constructible");
		
		class Promise;
		using CoroHandle = std::experimental::coroutine_handle<Promise>;

		class Promise
		{
		private:
			using CoroHandle = std::experimental::coroutine_handle<Promise>;
			using suspend_always = std::experimental::suspend_always;

		public:
			auto get_return_object() { return CoroHandle::from_promise(*this); }
			auto initial_suspend() { return suspend_always{}; }
			auto final_suspend() { return suspend_always{}; }
			auto yield_value(T value) {
				this->value = value;
				return suspend_always{};
			}
			void unhandled_exception() { std::terminate(); }
			const auto& current_value() { return value; }

		private:
			T value = 0;
		};

	public:
		using promise_type = Promise;

		Generator(CoroHandle&& handle) : coro_handle(std::move(handle)) { assert(coro_handle); }
		Generator(Generator&&) = default;
		Generator(const Generator&) = delete;
		~Generator() { coro_handle.destroy(); }

		Generator& operator=(Generator&&) = default;
		Generator& operator=(const Generator&) = delete;

		bool resume() {
			if (!coro_handle.done()) {
				coro_handle.resume();
			}
			return !coro_handle.done();
		}
		const auto& current_value() { return coro_handle.promise().current_value(); }

	private:
		CoroHandle coro_handle;
	};

	const auto make_generator = [](auto initial_value, auto f) {
		return [initial_value, f]() -> Generator<decltype(initial_value)> {
			auto value = initial_value;
			while (true) {
				co_yield value;
				value = f(value);
			}
		};
	};
	
	inline constexpr auto increment = [](auto x) { return ++x; };
	const auto from = [](auto initial) {
		return make_generator(std::move(initial), increment);
	};
	const auto nats = from(0u);
}