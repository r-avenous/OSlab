#include <stdio.h>
#include <ncurses.h>

int main()
{
    initscr();
    keypad(stdscr, TRUE);
    cbreak();

    noecho();
    int ch;

    while ((ch = getch()) != '\n'){

        if (ch == KEY_UP)
            printw("up");
        else if (ch == KEY_DOWN)
            printw("down");
        else if (ch == KEY_LEFT)
            printw("left");
        else if (ch == KEY_RIGHT)
            printw("right");
        else
            printw("%c", ch);

        refresh();
    }
    refresh();
    endwin();
    return 0;
}

