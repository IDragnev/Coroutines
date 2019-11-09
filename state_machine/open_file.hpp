#pragma once

#include "state_machine.hpp"
#include <string_view>

namespace Coroutines {
	enum class ButtonPressed
	{
		left_mouse,
		right_mouse,
		//...
	};

	struct File 
	{
		std::string name = "";
	};
	
	StateMachine<ButtonPressed, File> open_file(std::string_view filename);
}