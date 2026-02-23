#include "gpio_buttons.hpp"
#include <gpiod.hpp>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>

GPIOButtons::GPIOButtons() {}
GPIOButtons::~GPIOButtons() { stop(); }

void GPIOButtons::start() {
    running = true;
    worker = std::thread(&GPIOButtons::run, this);
}

void GPIOButtons::stop() {
    running = false;
    if (worker.joinable())
        worker.join();
}

void GPIOButtons::run() {
    gpiod::chip chip("gpiochip0");

    std::vector<unsigned int> offsets = {17, 27, 24, 23};

    gpiod::line_bulk lines = chip.get_lines(offsets);

    lines.request({
        "pi_buttons",
        gpiod::line_request::DIRECTION_INPUT,
        gpiod::line_request::FLAG_BIAS_PULL_UP
    });

    std::vector<int> last = {1,1,1,1};

    while (running) {
        auto values = lines.get_values();

        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i] == 0 && last[i] == 1) {
                if (i == 0) ungetch(KEY_UP);
                if (i == 1) ungetch(KEY_DOWN);
                if (i == 2) ungetch(10);
                if (i == 3) ungetch(KEY_LEFT);
            }
            last[i] = values[i];
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}