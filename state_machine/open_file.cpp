#include "open_file.hpp"

namespace Coroutines {
	StateMachine<ButtonPressed, File> open_file(std::string_view filename) {
		using Coroutine = StateMachine<ButtonPressed, File>;
		using Signal = Coroutine::Awaitable;

		auto first_button = co_await Signal{};
		while (true) {
			const auto second_button = co_await Signal{};
			if (first_button == ButtonPressed::left_mouse &&
				second_button == ButtonPressed::left_mouse) {
				co_return File{filename.data()};
			}
			else {
				first_button = second_button;
			}
		}
	}
}