#include <iostream>
#include <experimental/coroutine>
#include <assert.h>
#include <vector>

#include "generator.h"
#include "printer_coro.h"
#include "summator_coroutine.h"

int main()
{
	//coroutines created, but not active yet
	auto nats = Coroutines::nats();
	auto print_nats_ended = Coroutines::print_line("...nats ended!");
	
	auto nums = std::vector<std::uint32_t>{};
	//let's activate them
	while (nats.resume()) {
		const auto num = nats.current_value();
		std::cout << num << ' ';
		nums.push_back(num);
		if (num == 11) {
			break;
		}
	}
	//nats is still resumable here, as it is a generator
	
	print_nats_ended.resume();
	//here print_nats_ended is done and not resumable anymore
	
	//create a Summator coroutine
	auto summator = Coroutines::sum(nums);
	//summator is suspended here

	summator.resume();
	//summator is done here and cannot be resumed anymore
	//but we can access its result
	const auto sum = summator.result();
	const auto message = "sum = " + std::to_string(sum);

	//create a new printer coroutine
	auto print_sum = Coroutines::print_line(message);
	//print_sum is still inactive

	print_sum.resume();
	//print_sum is done here
} //they all die here