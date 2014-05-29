#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#define DELAY 125000

int main(int argc, char *argv[])
{
	int x = 0, y = 0;
	int max_y = 0, max_x = 0;
	int next_x = 0;
	int direction = 1;

	initscr();
	refresh();
	cbreak();
	noecho();
	clear();

	curs_set(0);

	// Global var `stdscr` is created by the call to `initscr()`
	getmaxyx(stdscr, max_y, max_x);
	char c[1024] = ">";

	while (1) {
		clear();

		mvaddstr(y, x, c);
		
		refresh();

		usleep(DELAY);

		next_x = x + direction;

		if (next_x >= max_x || next_x < 0) {
			direction *= -1;
		} else {
			x += direction;
		}

		strcat(c, ">");
	}

	endwin();
}
