#ifndef UI_PAGES_HPP
#define UI_PAGES_HPP

#include <ncurses.h>
#include "real_time_audio_processor.hpp"
#include "ui_types.hpp"

PageResult runMainMenuPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons);
PageResult runDeviceSelectPage(WINDOW* win, const UIContext& ctx, const std::vector<std::string>& devices, GPIOButtons& gpio_buttons);
PageResult runPlayAlongListPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons);
PageResult runPlayAlongPlayerPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons);
PageResult runSoloPlayerPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons);
PageResult runSummaryPage(WINDOW* win, const UIContext& ctx, GPIOButtons& gpio_buttons);

#endif // UI_PAGES_HPP