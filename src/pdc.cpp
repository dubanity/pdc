#define _WIN32_WINNT 0x0500

#include "curses.h"
#include <thread>
#include <chrono>
#include <sstream>
#include <array>
#include <Windows.h>
#include <WinUser.h>

using namespace std::literals::chrono_literals;

#ifdef __KEYBOARD_DEBUG
    int kbhit(void)
    {
        int ch = getch();

        if (ch != ERR) {
            ungetch(ch);
            return 1;
        }
        else {
            return 0;
        }
    }

    int main(void)
    {
        initscr();

        cbreak();
        noecho();
        nodelay(stdscr, TRUE);

        scrollok(stdscr, TRUE);
        while (1) {
            if (kbhit()) {
                printw("Key pressed! It was: %d\n", getch());
                refresh();
            }
            else {
                printw("No key pressed yet...\n");
                refresh();
                std::this_thread::sleep_for(1s);
            }
        }
    }
#else

CONSOLE_SCREEN_BUFFER_INFO csbi;
signed int COLS, ROWS;
int INPUT_YMAX, INPUT_XMAX;
int INPUT_YBEGIN, INPUT_XBEGIN;
int DISPLAY_YBEGIN, DISPLAY_XBEGIN;
int DISPLAY_YMAX, DISPLAY_XMAX;
int IDX_INPUT = INPUT_XBEGIN + 2;
const int O_IDX_INPUT = IDX_INPUT;
std::stringstream ssMsg;
std::string sMsg;
std::array<std::string, 28> qMsg;
int qMsgIndex = 0;

void clearDisplay(WINDOW* win);
void clearInput(WINDOW* win, int nS);
void qPrintAll(WINDOW* win);

int main()
{
    qMsg[28] = std::string("EOF");
    HWND chWindow = GetConsoleWindow();
    SetWindowLong(chWindow, GWL_STYLE, GetWindowLong(chWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    ROWS = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    COLS = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    RECT r;
    GetWindowRect(chWindow, &r);
    MoveWindow(chWindow, r.left, r.top, 1920/2, 1080/2, TRUE);
    initscr();
    raw();
    noecho();
#pragma warning(disable:4244)
    WINDOW* displayWin = newwin(COLS / 1.04, ROWS / 1.04, 0, 0);
#pragma warning(default:4244)
    getbegyx(displayWin, DISPLAY_YBEGIN, DISPLAY_XBEGIN);
    getmaxyx(displayWin, DISPLAY_YMAX, DISPLAY_XMAX);
    refresh();
    box(displayWin, 0, 0);
    mvwprintw(displayWin, 1, 1, "Welcome to PDCurses! This is a UI design test originally for a text-based chat system.");
    wrefresh(displayWin);
#pragma warning(disable:4244)
    WINDOW* inputWin = newwin(3, ROWS / 1.04, 28, 0);
#pragma warning(default:4244)
    getmaxyx(inputWin, INPUT_YMAX, INPUT_XMAX);
    refresh();
    box(inputWin, 0, 0);
    wrefresh(inputWin);
    mvwaddch(inputWin, 0, 1, '>');
    wrefresh(inputWin);
    mvwprintw(inputWin, 2, INPUT_XMAX - 5, "v1.1");
    wrefresh(inputWin);
    while (true)
    {
        char c = getch();
        int n = c;
        if (n == 8)
        {
            std::string t_ssMsg = ssMsg.str();
            sMsg = t_ssMsg.substr(0, t_ssMsg.size() - 1);
            std::stringstream().swap(ssMsg);
            ssMsg << sMsg;
        }
        else
        {
            ssMsg << c;
        }

        switch (n)
        {
        case 13:
            clearDisplay(displayWin);
            qMsg[qMsgIndex] = ssMsg.str();
            qMsgIndex++;
            qPrintAll(displayWin);
            clearInput(inputWin, 1);
            std::stringstream().swap(ssMsg);
            break;
        case 8:
            {
                clearInput(inputWin, 0);
                mvwprintw(inputWin, INPUT_YBEGIN + 1, O_IDX_INPUT, ssMsg.str().c_str());
                wrefresh(inputWin);
                IDX_INPUT--;
            }
            break;
        default:
            mvwaddch(inputWin, INPUT_YBEGIN + 1, IDX_INPUT, c);
            wrefresh(inputWin);
            IDX_INPUT++;
            break;
        }
    }
    getch();
    endwin();
    return 0;
}

void clearDisplay(WINDOW* win)
{
    for (int i = DISPLAY_XBEGIN + 1; i < DISPLAY_XMAX - 1; i++)
    {
        mvwaddch(win, DISPLAY_YBEGIN + 1, i, ' ');
        wrefresh(win);
    }
}

void clearInput(WINDOW* win, int nS)
{
    if (nS == 1)
    {
        for (int i = INPUT_XBEGIN + 1; i < INPUT_XMAX - 1; i++)
        {
            mvwaddch(win, INPUT_YBEGIN + 1, i, ' ');
            wrefresh(win);
            IDX_INPUT = O_IDX_INPUT;
        }
    }
    else if (nS == 0)
    {
        for (int i = INPUT_XBEGIN + 1; i < INPUT_XMAX - 1; i++)
        {
            mvwaddch(win, INPUT_YBEGIN + 1, i, ' ');
            wrefresh(win);
        }
    }
}

void qPrintAll(WINDOW* win)
{
    for (int i = 0; i < 28; i++)
    {
        if (i == 26)
        {
            return;
        }
        if (i == 0)
        {
            mvwprintw(win, DISPLAY_YBEGIN + 1, DISPLAY_XBEGIN + 1, qMsg[i].c_str());
            wrefresh(win);
        }
        if (i == 1)
        {
            mvwprintw(win, DISPLAY_YBEGIN + 2, DISPLAY_XBEGIN + 1, qMsg[i].c_str());
            wrefresh(win);
        }
        if (i > 1)
        {
            mvwprintw(win, DISPLAY_YBEGIN + i + 1, DISPLAY_XBEGIN + 1, qMsg[i].c_str());
            wrefresh(win);
        }
    }
}

#endif