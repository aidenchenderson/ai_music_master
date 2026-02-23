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

    int getKey();

private:
    void run();

    std::thread worker;
    std::atomic<bool> running{false};
    std::atomic<int> pending_key{-1};
};

#endif