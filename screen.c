#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>

#define DELAY 10

int main(int argc, char *argv[])
{
	int x = 0, y = 0;
	int max_y = 0, max_x = 0;
	int next_x = 0;
	int next_y = 0;
	int direction_x = 1;
	int direction_y = 1;

	setlocale(LC_ALL, "");

	initscr();
	refresh();
	cbreak();
	noecho();

	start_color();

	use_default_colors();
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_BLACK);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);
	init_pair(6, COLOR_YELLOW, COLOR_BLACK);
	init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(8, COLOR_CYAN, COLOR_BLACK);

	color_set(0, NULL);
	assume_default_colors(COLOR_WHITE, COLOR_BLACK);

	clear();

	curs_set(0);

	// Global var `stdscr` is created by the call to `initscr()`
	getmaxyx(stdscr, max_y, max_x);

	char *c = " ";
	char *bars = " ▁▂▃▃▄▅▇█";

	srand((unsigned)time(NULL));

	while (1) {
		mvaddstr(y, x, c);

		refresh();

		usleep(DELAY);

		next_x = x + direction_x;
		next_y = y + direction_y;

		if (next_x >= max_x || next_x < 0) {
			direction_x *= -1;
			if (x > 0) {
				int r = rand() % 8;
				attrset(A_NORMAL | COLOR_PAIR(r));
				c = " 𐌏 ";

			} else {
				c = " 𐌏 ";
			}
		} else {
			x += direction_x;
		}

		if (next_y >= max_y || next_y < 0) {
			direction_y *= -1;
			if (y > 0) {
				int r = rand() % 8;
				attrset(A_NORMAL | COLOR_PAIR(r));
				c = " 𐌏 ";

			} else {
				c = " 𐌏 ";
			}
		} else {
			y += direction_y;
		}
	}

	endwin();
}
