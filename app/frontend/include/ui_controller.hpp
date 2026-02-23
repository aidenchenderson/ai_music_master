#ifndef UI_CONTROLLER_HPP
#define UI_CONTROLLER_HPP

#include <string>
#include <ncurses.h>
#include "gpio_buttons.hpp"

void runMainUI();
int getInput(WINDOW* win, GPIOButtons& buttons);

#endif // UI_CONTROLLER_HPP