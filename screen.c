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

	int max_hits;

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

	r->max_hits = 0;

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

int Rock_collide(const struct Rock *a, const struct Rock *b)
{
	if (a->x == b->x && a->y == b->y) {
		return 1;
	}

	return 0;
}

void Rock_input(struct Rock *r, struct Rock **rockets, int ch)
{
	switch (ch) {
	case KEY_LEFT:
		r->direction_y = 0;
		r->direction_x = -1;
		r->ch = "<";
		break;

	case KEY_RIGHT:
		r->direction_y = 0;
		r->direction_x = 1;
		r->ch = ">";
		break;

	case KEY_UP:
		r->direction_y = -1;
		r->direction_x = 0;
		r->ch = "^";
		break;

	case KEY_DOWN:
		r->direction_y = 1;
		r->direction_x = 0;
		r->ch = "v";
		break;

	case ' ':
		if (!rockets[0]) {
			rockets[0] = Rock_create("-");
			rockets[0]->direction_x = r->direction_x * 2;
			rockets[0]->direction_y = r->direction_y * 2;
			rockets[0]->x = r->x;
			rockets[0]->y = r->y;
		}
		break;
	}
}

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));

	int i;

	int max_y = 0;
	int max_x = 0;

	struct Rock **rocks;

	rocks = malloc(MAX * sizeof(struct Rock));

	struct Rock **rockets;

	rockets = malloc(MAX * sizeof(struct Rock));

	if (!rocks) {
		printf("malloc error");
		exit(1);
	}

	if (!rockets) {
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

	clear();

	curs_set(0);

	getmaxyx(stdscr, max_y, max_x);

	WINDOW *field = newwin(max_y - 3, max_x, 0, 0);
	WINDOW *score = newwin(3, max_x, max_y - 3, 0);

	wrefresh(field);
	wrefresh(score);

	for (i = 1; i < MAX; i++) {
		rocks[i] = Rock_create("#");
		rocks[i]->x = rand() % max_x - 3;
		rocks[i]->y = rand() % max_y - 3;
	}

	int ch = 0;

	nodelay(stdscr, TRUE);

	int hits = 0;

	while (ch != 'q') {
		ch = getch();

		napms(100);

		wclear(field);

		Rock_move(rocks[0], max_x - 3, max_y - 3);

		if (rockets[0]) {
			Rock_move(rockets[0], max_x - 3, max_y - 3);
		}

		for (i = 1; i < MAX; i++) {
			Rock_move(rocks[i], max_x - 3, max_y - 3);

			if (Rock_collide(rocks[0], rocks[i])) {
				hits++;
				mvwprintw(score, 0, 0, "hits: %d", hits);
				wrefresh(score);
			}
		}

		for (i = 0; i < MAX; i++) {
			mvwprintw(field, rocks[i]->y, rocks[i]->x, rocks[i]->ch);
		}

		if(rockets[0]) {
			mvwprintw(field, rockets[0]->y, rockets[0]->x, rockets[0]->ch);
		}

		Rock_input(rocks[0], rockets, ch);

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

	delwin(field);
	delwin(score);

	endwin();
}
