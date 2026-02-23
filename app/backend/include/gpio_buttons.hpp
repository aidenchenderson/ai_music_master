#ifndef GPIO_BUTTONS_H
#define GPIO_BUTTONS_H

#include <thread>
#include <atomic>

class GPIOButtons {
public:
    GPIOButtons();
    ~GPIOButtons();

    void start();
    void stop();

private:
    void run();

    std::thread worker;
    std::atomic<bool> running{false};
};

#endif //GPIO_BUTTONS_H