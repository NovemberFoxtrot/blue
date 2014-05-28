#include <ncurses.h>
#include <unistd.h>

#define DELAY 25000

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
	char *c = ">";
	char *b = "#";

	while (1) {
		clear();

		if (x % 2 == 0) {
		} else if (direction < 0) {
			c = "<";
		} else {
			c = ">";
		}

		mvaddstr(y, x, c);
		mvaddstr(y + 3, x, c);
		mvaddstr(y + 4, x - 1, "/");
		mvaddstr(y + 7, x, c);
		mvaddstr(y + 11, x, c);
		mvaddstr(y + 13, x, c);
		mvaddstr(y + 17, x, c);
		mvaddstr(y + 19, x, c);
		mvaddstr(y + 23, x, c);

		refresh();

		usleep(DELAY);

		next_x = x + direction;

		if (next_x >= max_x || next_x < 0) {
			direction *= -1;

		} else {
			x += direction;
		}
	}

	endwin();
}
