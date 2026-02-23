#include "gpio_buttons.hpp"
#include <gpiod.hpp>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>
#include <map>

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

    gpiod::line_settings settings;
    settings.set_direction(gpiod::line::direction::INPUT);
    settings.set_bias(gpiod::line::bias::PULL_UP);

    gpiod::request_config config;
    config.set_consumer("pi_buttons");

    std::vector<unsigned int> offsets = {17, 27, 24, 23};

    std::map<unsigned int, gpiod::line_settings> line_config;
    for (auto offset : offsets)
        line_config[offset] = settings;

    auto request = chip.request_lines(config, line_config);

    std::vector<int> last = {1,1,1,1};

    while (running) {
        auto values = request.get_values();

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