#include <ncurses.h>
#include <locale.h>
#include <string>

const int HEIGHT = 34;
const int WIDTH = 136;
const int PAGE_COUNT = 3;

void drawPage(WINDOW * win, int page) 
{
    int height, width;
    getmaxyx(win, height, width);
    std::string labels[PAGE_COUNT];
    int totalWidth = 0;
    int x = (width - totalWidth) / 2;
    int y = height - 2;


    werase(win);
    box(win, 0, 0);
    
    if (page == 0) {
        mvwprintw(win, 1, 1, "Dltsh Manual - Page 1. Information about dltsh.");
        mvwprintw(win, 2, 1, "dltsh is a minimalist command-line shell written in C."); 
        mvwprintw(win, 3, 1, "It doesn't use GNU utilities; most basic commands are implemented from scratch.");    
        mvwprintw(win, 4, 1, "The entire dltsh project is built using Clang/LLVM. Clang for C/C++ and LLVM as the backend for the Rust compiler");
        mvwprintw(win, 5, 1, "");
    }
    else if (page == 1) {
        mvwprintw(win, 1, 1, "Dltsh Manual - Page 2. Commands.");
        mvwprintw(win, 2, 1,
                  "Base commands. Files and directories. And other Commands.");

        mvwprintw(win, 4, 1,  "1.help - output table with commands.");
        mvwprintw(win, 5, 1,  "  Usage: help. No flags or arguments.");
        mvwprintw(win, 7, 1,  "2.pwd - output path to the directory.");
        mvwprintw(win, 8, 1,  "  Usage: pwd. No flags or arguments.");
        mvwprintw(win, 10, 1, "3.rm - remove directory or file.");
        mvwprintw(win, 11, 1, "  Usage: rm <flag> <name for file or dir>. Flags: -f -> for file, -d -> for directory.");
        mvwprintw(win, 13, 1, "4.cd - change directory.");
        mvwprintw(win, 14, 1, "  Usage: cd <name>. Arguments: name for dir");
        mvwprintw(win, 16, 1, "5.ls - output files in directory.");
        mvwprintw(win, 17, 1, "  Usage: ls. No flags or arguments.");
        mvwprintw(win, 19, 1, "6.touch - creating file.");
        mvwprintw(win, 20, 1, "  Usage: touch <file name>.");
        mvwprintw(win, 22, 1, "7.mkdir - creating directory.");
        mvwprintw(win, 23, 1, "  Usage: mkdir <directory name>.");
        mvwprintw(win, 25, 1, "8.cat - output content in file.");
        mvwprintw(win, 26, 1, "  Usage: cat <file name>.");
        mvwprintw(win, 28, 1, "9.calc - calculator.");
        mvwprintw(win, 29, 1, "  Usage: calc. No flagd or arguments.");
    } 
    else {
        mvwprintw(win, 1, 1, "Dltsh Manual - Page 3. Commands.");
        mvwprintw(win, 2, 1, "Other commands.");

        mvwprintw(win, 4, 1,  "10.dex - small text editor.");
        mvwprintw(win, 5, 1,  "   Usage: dex. No flags or arguments");
        mvwprintw(win, 7, 1,  "11.clear - clear the screen.");
        mvwprintw(win, 8, 1,  "   Usage: clear. No flags or arguments.");
        mvwprintw(win, 10, 1, "12.dlt-fetch - system information.");
        mvwprintw(win, 11, 1, "   Usage: dlt-fetch. No flags or arguments.");
        mvwprintw(win, 13, 1, "13.exit - exit dltsh.");
        mvwprintw(win, 14, 1, "   Usage: exit. No flags or arguments.");
        mvwprintw(win, 16, 1, "14.ver - output dltsh version.");
        mvwprintw(win, 17, 1, "   Usage: ver. No flags or arguments"); 
        mvwprintw(win, 19, 1, "15.clocks - CLI/TUI Clocks on Rust.");
        mvwprintw(win, 20, 1, "   Usage: clocks. No flags or arguments.");
        mvwprintw(win, 22, 1, "16.manual - CLI/TUI manual. like `help` but there is more information on the commands and dltsh.");
        mvwprintw(win, 23, 1, "   usage: manual. No flags or arguments.");
    }

    mvwprintw(win, y, 1, "Press 'q' to exit");

    for (int i = 0; i < PAGE_COUNT; ++i) {
        labels[i] = (i == page)
            ? "[" + std::to_string(i + 1) + "]"
            : " " + std::to_string(i + 1) + " ";

        totalWidth += static_cast<int>(labels[i].size());
        if (i > 0) {
            totalWidth += 2;
        }
    }

    for (int i = 0; i < PAGE_COUNT; ++i) {
        if (i > 0) {
            x += 2;
        }

        if (i == page) {
            wattron(win, A_REVERSE);
        }

        mvwprintw(win, y, x, "%s", labels[i].c_str());

        if (i == page) {
            wattroff(win, A_REVERSE);
        }

        x += static_cast<int>(labels[i].size());
    }

    wrefresh(win);
}

int main()
{
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int starty = (LINES - HEIGHT) / 2;
    int startx = (COLS - WIDTH) / 2;

    WINDOW * win = newwin(HEIGHT, WIDTH, starty, startx);
    if (win == nullptr) {
        endwin();
        return 1;
    }

    keypad(win, TRUE);

    int currentPage = 0;
    drawPage(win, currentPage);

    bool running = true;

    while (running) {
        int key = wgetch(win);

        switch (key) {
            case KEY_LEFT:
                if (currentPage > 0) {
                    --currentPage;
                }
                break;

            case KEY_RIGHT:
                if (currentPage < PAGE_COUNT - 1) {
                    ++currentPage;
                }
                break;

            case 'q':
            case 'Q':
                running = false;
                break;
        }

        if (running) {
            drawPage(win, currentPage);
        }
    }

    delwin(win);
    endwin();
    return 0;
}
