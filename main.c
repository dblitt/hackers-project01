#include <ncurses.h>

int main() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();

    printw("type a character: ");
    int ch = getch();
    printw("\n");

    if (ch == KEY_F(1)) {
        printw("F1 pressed");
    } else {
        printw("The key was: ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }

    refresh();
    getch();
    endwin();

    return 0;
}
