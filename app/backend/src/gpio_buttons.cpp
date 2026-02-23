#include "gpio_buttons.hpp"
#include <gpiod.h>
#include <ncurses.h>
#include <chrono>
#include <thread>

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

    struct gpiod_chip* chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) return;

    unsigned int offsets[] = {17, 27, 24, 23};
    const int num_lines = 4;

    struct gpiod_line_config* line_cfg = gpiod_line_config_new();
    struct gpiod_line_settings* settings = gpiod_line_settings_new();
    struct gpiod_request_config* req_cfg = gpiod_request_config_new();

    if (!line_cfg || !settings || !req_cfg) {
        if (settings) gpiod_line_settings_free(settings);
        if (line_cfg) gpiod_line_config_free(line_cfg);
        if (req_cfg)  gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
        return;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);

    gpiod_line_config_add_line_settings(line_cfg, offsets, num_lines, settings);

    gpiod_request_config_set_consumer(req_cfg, "pi_buttons");

    struct gpiod_line_request* request =
        gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    if (!request) {
        gpiod_line_settings_free(settings);
        gpiod_line_config_free(line_cfg);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
        return;
    }

    int last[] = {1, 1, 1, 1};
    int values[4];

    while (running) {

        if (gpiod_line_request_get_values(request, values) == 0) {
            for (int i = 0; i < num_lines; ++i) {

                if (values[i] == 0 && last[i] == 1) {
                    if (i == 0) ungetch(KEY_UP);
                    if (i == 1) ungetch(KEY_DOWN);
                    if (i == 2) ungetch(10);
                    if (i == 3) ungetch(KEY_LEFT);
                }

                last[i] = values[i];
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    gpiod_line_request_release(request);
    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);
    gpiod_chip_close(chip);
}