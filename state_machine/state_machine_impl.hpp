#include "state_machine.hpp"
namespace Coroutines {
	template <typename Signal, typename Result>
	StateMachine<Signal, Result>::StateMachine(coro_handle&& handle)
		: coroutine_handle(std::move(handle))
	{
		assert(handle);
	}

	template <typename Signal, typename Result>
	inline StateMachine<Signal, Result>::~StateMachine() {
		coroutine_handle.destroy();
	}

	template <typename Signal, typename Result>
	void StateMachine<Signal, Result>::send_signal(Signal signal) {
		coroutine_handle.promise().store_as_recent(std::move(signal));
		if (!coroutine_handle.done()) {
			coroutine_handle.resume();
		}
	}

	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::result() const& -> std::optional<Result> {
		return coroutine_handle.promise().get_result();
	}

	template <typename Signal, typename Result>
	inline auto StateMachine<Signal, Result>::result() && -> std::optional<Result> {
		auto& promise = coroutine_handle.promise();
		return std::move(promise).get_result();
	}
}