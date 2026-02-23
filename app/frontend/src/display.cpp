#include <ncurses.h>
#include <string>
#include "audio_engine.hpp"
#include "ui_controller.hpp"
#include "ui_pages.hpp"
#include "ui_types.hpp"
#include "gpio_buttons.hpp"

void runMainUI() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    GPIOButtons buttons;
    buttons.start();

    int yMax = 15;
    int xMax = 50;
    
    WINDOW *win = newwin(yMax, xMax, 0, 0); 
    keypad(win, TRUE);

    wbkgd(win, A_BOLD);

    UIContext ctx;
    ctx.selectedDevice = "No device selected";
    std::vector<std::string> capture_devices = AudioEngine::get_capture_devices();

    PageId current = PageId::MainMenu;
    while (current != PageId::Exit) {
        PageResult result;

        switch (current) {
            case PageId::MainMenu:
                result = runMainMenuPage(win, ctx, buttons);
                break;
            case PageId::DeviceSelect:
                result = runDeviceSelectPage(win, ctx, capture_devices, buttons);
                break;
            case PageId::PlayAlongList:
                result = runPlayAlongListPage(win, ctx, buttons);
                break;
            case PageId::PlayAlongPlayer:
                result = runPlayAlongPlayerPage(win, ctx, buttons);
                break;
            case PageId::SoloStart:
                result = runSoloPlayerPage(win, ctx, buttons);
                break;
            case PageId::Summary:
                result = runSummaryPage(win, ctx, buttons);
                break;
            case PageId::Exit:
                result = {PageId::Exit, ctx};
                break;
        }

        ctx = result.context;
        current = result.nextPage;
    }

    delwin(win);
    buttons.stop();
    endwin();
}

int getInput(WINDOW* win, GPIOButtons& buttons) {
    int hw = buttons.getKey();
    if (hw != -1) {
        ungetch(hw);
    }

    return wgetch(win);
}