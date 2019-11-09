#pragma once

#include <experimental/coroutine>
#include <assert.h>
#include <numeric>

namespace Coroutines {
	class Summator
	{
	private:
		class Promise
		{
		private:
			using CoroHandle = std::experimental::coroutine_handle<Promise>;
			using suspend_always = std::experimental::suspend_always;

		public:
			auto get_return_object() { return CoroHandle::from_promise(*this); }
			auto initial_suspend() const { return suspend_always{}; }
			auto final_suspend() const { return suspend_always{}; }
			void unhandled_exception() const { std::terminate(); }
			void return_value(std::int64_t sum) { this->sum = sum; }

			auto result() const { return sum; }
		private:
			std::int64_t sum = 0;
		};
		using CoroHandle = std::experimental::coroutine_handle<Promise>;

	public:
		using promise_type = Promise;

		Summator(CoroHandle&& handle) : coro_handle(std::move(handle)) { assert(coro_handle); }
		Summator(Summator&&) = default;
		Summator(const Summator&) = delete;
		~Summator() { coro_handle.destroy(); }

		Summator& operator=(Summator&&) = default;
		Summator& operator=(const Summator&) = delete;

		bool resume() {
			if (!coro_handle.done()) {
				coro_handle.resume();
			}
			return !coro_handle.done();
		}

		auto result() const { return coro_handle.promise().result(); }

	private:
		CoroHandle coro_handle;
	};

	//take range by value since it may die
	//before the coroutine is resumed
	template <typename Range>
	auto sum(Range range) -> Summator {
		//initial suspend happens here

		//coroutine is resumed here
		const auto result = std::accumulate(range.cbegin(), range.cend(), 0);

		//after co_return, the coroutine reaches the final suspend point
		//which means it is done but not finished
		//and result is stored in its promise
		co_return result;
	}
}