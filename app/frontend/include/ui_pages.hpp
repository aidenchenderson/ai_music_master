#ifndef UI_PAGES_HPP
#define UI_PAGES_HPP

#include <ncurses.h>
//#include "session_recorder.hpp"
#include "ui_types.hpp"

PageResult runMainMenuPage(WINDOW* win, const UIContext& ctx);
PageResult runDeviceSelectPage(WINDOW* win, const UIContext& ctx, const std::vector<std::string>& devices);
PageResult runPlayAlongListPage(WINDOW* win, const UIContext& ctx);
PageResult runPlayAlongPlayerPage(WINDOW* win, const UIContext& ctx);
PageResult runSoloPlayerPage(WINDOW* win, const UIContext& ctx);
PageResult runSummaryPage(WINDOW* win, const UIContext& ctx);

#endif // UI_PAGES_HPP