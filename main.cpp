#include "BlockFall.h"
#include "GameController.h"
#include <curses.h>
#include <clocale>

int main() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();

    int h,w;
    getmaxyx(stdscr,h,w);
    mvprintw(h/2-1,w/2-21,"Enter your nickname: ");
    char name[20];
    getstr(name);

    const char* blocks[] = {"sourcefiles/blocks1.dat","sourcefiles/blocks2.dat","sourcefiles/blocks3.dat"};
    srand(static_cast<unsigned int>(time(0)));
    std::string blockFile = blocks[(rand()%3)];

    BlockFall game("sourcefiles/grid.dat", blockFile, false, "sourcefiles/leaderboard.txt", name);
    GameController controller;
    refresh();
    noecho();
    controller.play(game);
    while((int)(getch()!=111)); //won't close until o key

    endwin();
}
