#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>

#define DELAY 50

#define NEW_COLOR 1
#define RED 1000
#define GREEN 750
#define BLUE 750

struct Rock
{
	int x;
	int y;

	int next_x;
	int next_y;

	int direction_x;
	int direction_y;

	char *ch;
};

struct Rock *Rock_create(char *ch)
{
	struct Rock *r = malloc(sizeof(struct Rock));

	if (!r) {
		printf("malloc error");
		exit(1);
	}

	r->x = rand() % 10;
	r->y = rand() % 10;

	r->next_x = 0;
	r->next_y = 0;

	r->direction_x = 1;
	r->direction_y = 1;

	r->ch = ch;

	return r;
}

void Rock_move(struct Rock *r, int max_x, int max_y)
{
	r->next_x = r->x + r->direction_x;
	r->next_y = r->y + r->direction_y;

	if (r->next_x >= max_x || r->next_x < 0) {
		r->direction_x *= -1;
	} else {
		r->x += r->direction_x;
	}

	if (r->next_y >= max_y || r->next_y < 0) {
		r->direction_y *= -1;
	} else {
		r->y += r->direction_y;
	}
}

int main(int argc, char *argv[])
{
	int max_y = 0;
	int max_x = 0;

	struct Rock *ship = Rock_create("S");
	struct Rock *rock = Rock_create("R");

	if (!rock) {
		printf("malloc error");
		exit(1);
	}

	setlocale(LC_ALL, "");

	initscr();

	refresh();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	start_color();

	use_default_colors();

	init_color(NEW_COLOR, RED, GREEN, BLUE);

	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, NEW_COLOR, COLOR_BLACK);
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

	getmaxyx(stdscr, max_y, max_x);

	int ch = 0;

	srand((unsigned)time(NULL));

	refresh();

	nodelay(stdscr, TRUE);

	while (ch != 'q') {
		ch = getch();

		clear();

		mvprintw(0, 0, "max_y:%d max_x:%d y:%d x:%d", max_y, max_x, ship->y, ship->x);

		mvaddstr(ship->y, ship->x, ship->ch);
		mvaddstr(rock->y, rock->x, rock->ch);

		refresh();

		napms(100);

		if (ch == KEY_LEFT) {
			ship->direction_y = 0;
			ship->direction_x = -1;
		}

		if (ch == KEY_RIGHT) {
			ship->direction_y = 0;
			ship->direction_x = 1;
		}

		if (ch == KEY_UP) {
			ship->direction_y = -1;
			ship->direction_x = 0;
		}

		if (ch == KEY_DOWN) {
			ship->direction_y = 1;
			ship->direction_x = 0;
		}

		Rock_move(ship, max_x, max_y);
		Rock_move(rock, max_x, max_y);

		if (ship->x == rock->x && ship->y == rock->y) {
			mvaddstr(0,0,"Dude!");
		napms(1000);
			break;
		}
	}

	if(ship) free(ship);
	if(rock) free(rock);

	endwin();
}
