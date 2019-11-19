#include <iostream>
#include <experimental/coroutine>
#include <thread>
#include <future>
#include <chrono>
#include <assert.h>

using namespace std::chrono_literals;

struct thread_pool {
    template <typename T>
    static void launch_task(T&& task) {
        auto t = std::thread{ std::move(task) };
        t.detach();
    }
};

struct suspend_background {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::experimental::coroutine_handle<> h) const { 
        thread_pool::launch_task([coro = std::move(h)] {
            coro.resume(); 
        });
    }
    void await_resume() const noexcept { }
};

struct my_coro {
    struct promise_type;
    using coro_handle = std::experimental::coroutine_handle<promise_type>;

    struct promise_type {
        auto initial_suspend() {
            return std::experimental::suspend_never{};
        }
        auto final_suspend() {
            return std::experimental::suspend_always{};
        }
        auto get_return_object() {
            return coro_handle::from_promise(*this);
        }
        void unhandled_exception() { std::terminate(); }
        void return_value(int x) { value = x; }
        auto stored_val() const noexcept { return value; }
    private:
        int value = 0;
    };

    my_coro() = default;
    my_coro(my_coro&&) = default;
    my_coro(const my_coro&) = delete;
    ~my_coro() {
        if (coro) {
            coro.destroy();
        } 
    }
    my_coro(std::experimental::coroutine_handle<promise_type>&& h) : coro(std::move(h)) { assert(coro); }

    my_coro& operator=(my_coro&&) = default;
    my_coro& operator=(const my_coro&) = delete;

    bool resume() {
        if (!coro.done()) {
            coro.resume();
        }
        return !coro.done();
    }

    auto ret_val() const noexcept { return coro.promise().stored_val(); }

private:
    std::experimental::coroutine_handle<promise_type> coro;
};

auto thread_id() {
    return std::this_thread::get_id();
}

my_coro coro() {
	std::cout << "entering coro...";
    std::cout << "thread id: " << thread_id() << "\n";
    std::cout << "suspending...\n";
    co_await suspend_background{};
    std::cout << "resuming in coro...";
    std::cout << "(thread id: " << thread_id() << ")\n";
    std::cout << "leaving coro by returning 2\n";
    co_return 2;
}

int main() {
	std::cout << "launching the coroutine in main...\n";
    auto fun = coro();
	std::cout << "waiting for the coroutine to finish in main...";
	std::cout << "(tid: " << thread_id() << ")\n";
    std::this_thread::sleep_for(5s);
    std::cout << "\nin main -> return value: " << fun.ret_val() << "\n";
}