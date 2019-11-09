namespace Coroutines {
	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::promise_type::get_return_object() -> StateMachine {
		return coro_handle::from_promise(*this);
	}

	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::promise_type::initial_suspend() const noexcept -> suspend_never {
		return {};
	}

	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::promise_type::final_suspend() const noexcept -> suspend_always {
		return {};
	}

	template <typename Signal, typename Result>
	void StateMachine<Signal, Result>::promise_type::unhandled_exception() {
		auto exceptionPtr = std::current_exception();
		if (exceptionPtr) {
			std::rethrow_exception(exceptionPtr);
		}
	}

	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::promise_type::await_transform(SignalAwaitable) -> SignalAwaiter {
		return SignalAwaiter{recent_signal};
	}

	template <typename Signal, typename Result>
	inline void StateMachine<Signal, Result>::promise_type::return_value(Result value) {
		result.emplace(std::move(value));
	}

	template <typename Signal, typename Result>
	inline std::optional<Result> StateMachine<Signal, Result>::promise_type::get_result() && {
		return std::move(result);
	}

	template <typename Signal, typename Result>
	inline std::optional<Result> StateMachine<Signal, Result>::promise_type::get_result() const& {
		return result;
	}
	
	template <typename Signal, typename Result>
	inline void StateMachine<Signal, Result>::promise_type::store_as_recent(Signal s) {
		recent_signal.emplace(s);
	}
}