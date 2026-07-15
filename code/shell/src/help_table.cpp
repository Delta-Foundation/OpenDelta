#include <ncurses.h>
#include <locale.h>
#include <string>

using namespace std;

int main()
{
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int height = 28;
    int width = 138;
    int starty = (LINES - height) / 4;
    int startx = (COLS - width) / 4;

    WINDOW *win = newwin(height, width, starty, startx);
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "Dtlsh Commands");

    mvwvline(win, 3, 38, ACS_VLINE, 6);

    mvwprintw(win, 3, 1,  "1.help - output table with commands;");
    mvwprintw(win, 4, 1,  "3.pwd - output path to the directory;");
    mvwprintw(win, 5, 1,  "5.rm - remove directory or file;");
    mvwprintw(win, 6, 1,  "7.cd - change directory;");
    mvwprintw(win, 7, 1,  "9.ver - output dltsh version;");
    mvwprintw(win, 8, 1,  "11.calc - calculator;");

    mvwprintw(win, 3, 40, "2.ls - output files in this directory");
    mvwprintw(win, 4, 40, "4.cat - output content in file.");
    mvwprintw(win, 5, 40, "6.touch - add file;");
    mvwprintw(win, 6, 40, "8.dex - small editor");
    mvwprintw(win, 7, 40, "10.clear - clear screen");
    mvwprintw(win, 8, 40, "12.dltinfo - like neofetch, but for dltsh");

    mvwvline(win, 3, 85, ACS_VLINE, 6);

    mvwprintw(win, 3, 87, "13.exit - exit dltsh");

    mvwprintw(win, 10, 1, "Dltsh Commands on rust");
    mvwprintw(win, 11, 1, "1.dexide - dex rewritten on rust and without crates; 1.calcrs - calculator on rust");
    mvwprintw(win, 12, 1, "3.clocks");

    mvwprintw(win, 14, 1, "Enter 'q' for exit");
    wrefresh(win);

    while (wgetch(win) != 'q') {}

    delwin(win);
    endwin();
    return 0;
}
