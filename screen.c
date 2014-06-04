#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <locale.h>

#include "object.h"

#define DELAY 100
#define MAX 10

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));

	int i;
	int j;

	int max_y = 0;
	int max_x = 0;

	struct Object *ship = Object_create(">", SHIP);

	struct Object **rocks;

	rocks = malloc(MAX * sizeof(struct Object));

	if (!rocks) {
		printf("malloc error");
		exit(1);
	}

	struct Object **rockets;

	rockets = malloc(MAXWEAPONS * sizeof(struct Object));

	if (!rockets) {
		printf("malloc error");
		exit(1);
	}

	setlocale(LC_ALL, "");

	initscr();

	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	clear();

	curs_set(0);

	getmaxyx(stdscr, max_y, max_x);

	WINDOW *field = newwin(max_y - 3, max_x, 0, 0);
	WINDOW *score = newwin(3, max_x, max_y - 3, 0);

	mvwprintw(score, 0, 0, "hits: %d", 0);

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
		ch = getch();

		flushinp();

		napms(DELAY);

		wclear(field);

		Object_move(ship, max_x - 3, max_y - 3);

		for (i = 0; i < MAX; i++) {
			Object_move(rocks[i], max_x - 3, max_y - 3);

			if (Object_collide(ship, rocks[i])) {
				hits++;
				mvwprintw(score, 0, 0, "hits: %d", hits);
				wrefresh(score);
			}
		}

		for (i = 0; i < MAX; i++) {
			Object_move(rocks[i], max_x - 3, max_y - 3);

			if (Object_collide(ship, rocks[i])) {
				hits++;
				mvwprintw(score, 0, 0, "hits: %d", hits);
				wrefresh(score);
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
						wrefresh(field);
					}
				}
			}
		}

		mvwprintw(field, ship->y, ship->x, ship->ch);

		
		mvwprintw(field, ship->y-1, ship->x, "  |\\ ");
		mvwprintw(field, ship->y, ship->x,   "<:||)");
		mvwprintw(field, ship->y+1, ship->x, "  |/");

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

		wrefresh(field);
	}

	if (rocks) {
		for (i = 0; i < MAX; i++) {
			if (rocks[i]) {
				free(rocks[i]);
			}
		}

		free(rocks);
	}

	if (rockets) {
		for (i = 0; i < MAX; i++) {
			if (rockets[i]) {
				free(rockets[i]);
			}
		}

		free(rockets);
	}

	if (ship) {
		free(ship);
	}

	delwin(field);
	delwin(score);

	endwin();
}
