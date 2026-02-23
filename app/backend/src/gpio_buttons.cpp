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

    gpiod::chip chip("4", gpiod::chip::OPEN_BY_NUMBER);

    std::vector<unsigned int> offsets = {17, 27, 24, 23};

    gpiod::line_bulk lines = chip.get_lines(offsets);
    lines.request({
        "pi_buttons",
        gpiod::line_request::DIRECTION_INPUT,
        gpiod::line_request::FLAG_BIAS_PULL_UP
    });

    while (running) {
        auto values = lines.get_values();

        if (values[0] == 0) ungetch(KEY_UP);
        if (values[1] == 0) ungetch(KEY_DOWN);
        if (values[2] == 0) ungetch(10);        // ENTER
        if (values[3] == 0) ungetch(KEY_LEFT);  // ESC/back

        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}