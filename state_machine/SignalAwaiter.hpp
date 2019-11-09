#pragma once

namespace Coroutines {
	template <typename Signal, typename Result>
	StateMachine<Signal, Result>::SignalAwaiter::SignalAwaiter(std::optional<Signal>& signal) noexcept
		: recent_signal(signal)
	{
	}

	template <typename Signal, typename Result>
	inline bool StateMachine<Signal, Result>::SignalAwaiter::await_ready() const noexcept {
		return recent_signal.has_value();
	}

	template <typename Signal, typename Result>
	inline void StateMachine<Signal, Result>::SignalAwaiter::await_suspend(coro_handle handle) const noexcept { }

	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::SignalAwaiter::await_resume() -> Signal {
		assert(recent_signal.has_value());
		return std::move(recent_signal).value();
	}

	template <typename Signal, typename Result>
	inline void StateMachine<Signal, Result>::SignalAwaiter::store_as_recent(Signal signal) {
		recent_signal = signal;
	}

	template <typename Signal, typename Result>
	inline std::optional<Signal> StateMachine<Signal, Result>::SignalAwaiter::get_recent_signal() const {
		return recent_signal;
	}
}