#include "gpio_buttons.hpp"
#include <gpiod.h>
#include <ncurses.h>
#include <iostream>

GPIOButtons::GPIOButtons() {}

GPIOButtons::~GPIOButtons() {
    stop();
}

void GPIOButtons::start() {
    if (running) return;
    running = true;
    worker = std::thread(&GPIOButtons::run, this);
}

void GPIOButtons::stop() {
    running = false;
    if (worker.joinable()) worker.join();
}

int GPIOButtons::getKey() {
    return pending_key.exchange(-1);
}

void GPIOButtons::run() {
    gpiod_chip* chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        std::cerr << "Failed to open gpiochip0\n";
        return;
    }

    unsigned int offsets[] = {17, 27, 24, 23};
    const int num_lines = 4;

    gpiod_line_settings* settings = gpiod_line_settings_new();
    gpiod_line_config* line_cfg = gpiod_line_config_new();
    gpiod_request_config* req_cfg = gpiod_request_config_new();

    if (!settings || !line_cfg || !req_cfg) {
        gpiod_chip_close(chip);
        return;
    }

    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);
    gpiod_line_settings_set_edge_detection(settings, GPIOD_LINE_EDGE_FALLING);

    gpiod_line_config_add_line_settings(line_cfg, offsets, num_lines, settings);
    gpiod_request_config_set_consumer(req_cfg, "pi_buttons");

    gpiod_line_request* request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);

    if (!request) {
        gpiod_chip_close(chip);
        return;
    }

    gpiod_edge_event_buffer* buffer = gpiod_edge_event_buffer_new(8);

    if (!buffer) {
        gpiod_line_request_release(request);
        gpiod_chip_close(chip);
        return;
    }

    while (running) {
        int ret = gpiod_line_request_wait_edge_events(request, -1);
        if (ret <= 0) continue;

        int events = gpiod_line_request_read_edge_events(request, buffer, 8);

        for (int i = 0; i < events; ++i) {
            gpiod_edge_event* event = gpiod_edge_event_buffer_get_event(buffer, i);

            unsigned int offset = gpiod_edge_event_get_line_offset(event);

            if (offset == 17) pending_key.store(KEY_UP);
            if (offset == 27) pending_key.store(KEY_DOWN);
            if (offset == 24) pending_key.store(10);
            if (offset == 23) pending_key.store(KEY_LEFT);
        }
    }

    gpiod_edge_event_buffer_free(buffer);
    gpiod_line_request_release(request);
    gpiod_line_settings_free(settings);
    gpiod_line_config_free(line_cfg);
    gpiod_request_config_free(req_cfg);
    gpiod_chip_close(chip);
}