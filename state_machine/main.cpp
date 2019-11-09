#include <iostream>
#include "state_machine.hpp"
#include "open_file.hpp"

namespace coro = Coroutines;

using ButtonPressed = coro::ButtonPressed;

int main() {
	auto machine = coro::open_file("some_fancy_file.txt");
	machine.send_signal(ButtonPressed::left_mouse);
	machine.send_signal(ButtonPressed::right_mouse);
	machine.send_signal(ButtonPressed::left_mouse);
	machine.send_signal(ButtonPressed::left_mouse);
	auto file_opt = machine.result();
	
	std::cout << file_opt.value().name << std::endl;

	return 0;
}
