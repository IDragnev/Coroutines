#pragma once 

#include <experimental/coroutine>
#include <optional>
#include <assert.h>

namespace Coroutines {
	template <typename Signal, typename Result>
	class StateMachine
	{
	public:
		class promise_type;

	private:
		using coro_handle = std::experimental::coroutine_handle<promise_type>;

		struct SignalAwaitable { };

		class SignalAwaiter
		{
		public:
			SignalAwaiter(std::optional<Signal>& signal) noexcept;

			bool await_ready() const noexcept;
			void await_suspend(coro_handle handle) const noexcept;
			Signal await_resume();

			void store_as_recent(Signal signal);
			std::optional<Signal> get_recent_signal() const;

		private:
			std::optional<Signal>& recent_signal;
		};

		class promise_type 
		{
		private:
			using suspend_always = std::experimental::suspend_always;
			using suspend_never = std::experimental::suspend_never;

		public:
			StateMachine get_return_object();
			suspend_never initial_suspend() const noexcept;
			suspend_always final_suspend() const noexcept;
			SignalAwaiter await_transform(SignalAwaitable);
			void return_value(Result value);
			void unhandled_exception();

			void store_as_recent(Signal s);
			std::optional<Result> get_result() &&;
			std::optional<Result> get_result() const&;

		private:
			std::optional<Signal> recent_signal;
			std::optional<Result> result;
		};

	public:
		using Awaitable = SignalAwaitable;

		StateMachine(coro_handle&& handle);
		StateMachine(StateMachine&&) = default;
		StateMachine(const StateMachine&) = delete;
		~StateMachine();

		StateMachine& operator=(StateMachine&&) = default;
		StateMachine& operator=(const StateMachine&) = delete;

		void send_signal(Signal signal);
		std::optional<Result> result() const&;
		std::optional<Result> result() &&;

	private:
		coro_handle coroutine_handle;
	};
}

#include "state_machine_impl.hpp"
#include "SignalAwaiter.hpp"
#include "promise_type.hpp"	