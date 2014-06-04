#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ncurses.h>
#include <locale.h>
#include <fcntl.h>

#include "object.h"

#define DELAY 50
#define MAX 10

struct Object **blue_array_create(uint32_t array_size)
{
	struct Object **array;

	array = malloc(array_size * sizeof(struct Object));

	if (!array) {
		printf("malloc error");
		exit(1);
	}

	return array;
}

void blue_array_clean(struct Object **array, uint32_t array_size) {
	if (array) {
		for (uint32_t i = 0; i < array_size; i++) {
			if (array[i]) {
				free(array[i]);
			}
		}
	}
}

void blue_array_destroy(struct Object **array) {
	if (array) {
		free(array);
	}
}

void blue_game_print_ship() {
}

int main()
{
	int i;
	int j;

	int max_y = 0;
	int max_x = 0;

	setlocale(LC_ALL, "");

	srand((unsigned)time(NULL));

	struct Object *ship = Object_create(">", SHIP);
	struct Object **rocks = blue_array_create(MAX);
	struct Object **rockets = blue_array_create(MAXWEAPONS);

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	int fd_flags = fcntl(0, F_GETFL);
  fcntl(0, F_SETFL, (fd_flags|O_NONBLOCK));

	getmaxyx(stdscr, max_y, max_x);

	WINDOW *field = newwin(max_y - 3, max_x, 0, 0);
	WINDOW *score = newwin(3, max_x, max_y - 3, 0);

	mvwprintw(score, 0, 0, "hits: %d", 0);
	wborder(score, 0,0,0,0,0,0,0,0);

	wrefresh(field);
	wrefresh(score);

	for (i = 0; i < MAX; i++) {
		rocks[i] = Object_create("░", ROCK);
		rocks[i]->x = rand() % max_x - 3;
		rocks[i]->y = rand() % max_y - 3;
	}

	int ch = 0;

	nodelay(stdscr, TRUE);

	int hits = 0;

	while (ch != 'q') {
		wrefresh(field);
		wrefresh(score);

		ch = getch();
		flushinp();

		napms(DELAY);

		wclear(field);
		wclear(score);

		wborder(field, 1, 1, 0, 0, 1, 1, 1, 1);
		wborder(score, 0, 0, 0, 0, 0, 0, 0, 0);

		mvwprintw(score, 1, 1, "hits: %d", hits);

		Object_move(ship, max_x - 3, max_y - 3);

		for (i = 0; i < MAX; i++) {
			Object_move(rocks[i], max_x - 3, max_y - 3);

			if (Object_collide(ship, rocks[i])) {
				hits++;
			}
		}

		for (i = 0; i < MAXWEAPONS; i++ ) {
			if (rockets[i]) {
				for (j = 0; j < MAX; j++) {
					if (Object_collide(rockets[i], rocks[j])) {
						rocks[j]->direction_x = 0;
						rocks[j]->direction_y = 0;
						rocks[j]->x = -1;
						rocks[j]->y = -1;
					}
				}
			}
		}

		mvwprintw(field, ship->y - 1, ship->x, "  |\\");
		mvwprintw(field, ship->y, ship->x,     "<:||)");
		mvwprintw(field, ship->y + 1, ship->x, "  |/");

		for (i = 0; i < MAX; i++) {
			mvwprintw(field, rocks[i]->y, rocks[i]->x, rocks[i]->ch);
		}

		for (i = 0; i < MAXWEAPONS; i++ ) {
			if (rockets[i]) {
				Object_move(rockets[i], max_x - 3, max_y - 3);
				mvwprintw(field, rockets[i]->y, rockets[i]->x, rockets[i]->ch);
			}
		}

		Object_input(ship, rockets, ch);
	}

	blue_array_clean(rocks, MAX);
	blue_array_destroy(rocks);

	blue_array_clean(rockets, MAX);
	blue_array_destroy(rockets);

	if (ship) {
		free(ship);
	}

	delwin(field);
	delwin(score);

	endwin();
}
