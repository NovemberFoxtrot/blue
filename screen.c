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

#define MAX 100

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

	r->direction_x = (rand() % 3) - 1;
	r->direction_y = (rand() % 3) - 1;

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
	int i;

	int max_y = 0;
	int max_x = 0;

	struct Rock **rocks;

	rocks = malloc(MAX * sizeof(struct Rock));

	if (!rocks) {
		printf("malloc error");
		exit(1);
	}

	rocks[0] = Rock_create(">");

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

	for (i = 1; i < MAX; i++) {
		rocks[i] = Rock_create("#");
		rocks[i]->x = rand() % max_x;
		rocks[i]->y = rand() % max_y;
	}

	int ch = 0;

	srand((unsigned)time(NULL));

	refresh();

	nodelay(stdscr, TRUE);

	while (ch != 'q') {
		ch = getch();

		clear();

		for (i = 0; i < MAX; i++) {
			mvaddstr(rocks[i]->y, rocks[i]->x, rocks[i]->ch);
		}

		refresh();

		napms(100);

		if (ch == KEY_LEFT) {
			rocks[0]->direction_y = 0;
			rocks[0]->direction_x = -1;
		}

		if (ch == KEY_RIGHT) {
			rocks[0]->direction_y = 0;
			rocks[0]->direction_x = 1;
		}

		if (ch == KEY_UP) {
			rocks[0]->direction_y = -1;
			rocks[0]->direction_x = 0;
		}

		if (ch == KEY_DOWN) {
			rocks[0]->direction_y = 1;
			rocks[0]->direction_x = 0;
		}

		if (ch == ' ') {
			rocks[0]->direction_y = 1;
			rocks[0]->direction_x = 0;
		}

		Rock_move(rocks[0], max_x, max_y);

		for (i = 1; i < MAX; i++) {
			Rock_move(rocks[i], max_x, max_y);

			if (rocks[0]->x == rocks[i]->x && rocks[0]->y == rocks[i]->y) {
				mvaddstr(0, 0, "Dude!");
				napms(1000);
			}
		}
	}

	if (rocks) {
		for (i = 0; i < MAX; i++) {
			if (rocks[i]) {
				free(rocks[i]);
			}
		}
		free(rocks);
	}

	endwin();
}
