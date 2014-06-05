#include <curses.h>
#include <fcntl.h>
#include <locale.h>
#include <math.h>
#include <ncurses.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define DELAY 50
#define MAX 10
#define MAXWEAPONS 100
#define BLUE_SPACE_HEIGHT 10
#define BLUE_SCORE_HEIGHT 3

enum ObjectType { SHIP = 0, WEAPON, ROCK, ALIEN, PLANET };

struct Object
{
	enum ObjectType type;

	int x;
	int y;

	int next_x;
	int next_y;

	int direction_x;
	int direction_y;

	int dimension_x;
	int dimension_y;

	int hits;

	char *ch;
};

struct Object *Object_create(char *ch, enum ObjectType type);
void Object_move(struct Object *o, int max_x, int max_y);
int Object_collide(const struct Object *a, const struct Object *b);
void Object_input(struct Object *o, struct Object **rockets, int ch);

const char *ship_design = ""
			  "  |\\ "
			  "<:||)"
			  "  |/ ";

struct Object *Object_create(char *ch, enum ObjectType type)
{
	struct Object *o = malloc(sizeof(struct Object));

	if (!o) {
		printf("malloc error");
		exit(1);
	}

	o->type = type;

	o->x = rand() % 10;
	o->y = rand() % 10;

	o->next_x = 0;
	o->next_y = 0;

	o->direction_x = (rand() % 3) - 1;
	o->direction_y = (rand() % 3) - 1;

	o->hits = 0;

	o->ch = ch;

	return o;
}

void Object_move(struct Object *o, int max_x, int max_y)
{
	o->next_x = o->x + o->direction_x;
	o->next_y = o->y + o->direction_y;

	if (o->type != WEAPON) {
		if (o->next_x >= max_x || o->next_x < 2) {
			o->direction_x *= -1;
		} else {
			o->x += o->direction_x;
		}

		if (o->next_y >= max_y || o->next_y < 2) {
			o->direction_y *= -1;
		} else {
			o->y += o->direction_y;
		}
	} else {
		if (o->next_x >= max_x || o->next_x < 2) {
			o->x = -1;
			o->y = -1;
			o->direction_x = 0;
			o->direction_y = 0;
		} else {
			o->x += o->direction_x;
		}

		if (o->next_y >= max_y || o->next_y < 2) {
			o->x = -1;
			o->y = -1;
			o->direction_x = 0;
			o->direction_y = 0;
		} else {
			o->y += o->direction_y;
		}
	}
}

char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
			   float p2_x, float p2_y, float p3_x, float p3_y,
			   float *i_x, float *i_y)
{
	float s1_x, s1_y, s2_x, s2_y;

	s1_x = p1_x - p0_x;
	s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;
	s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) /
	    (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) /
	    (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		if (i_x != NULL)
			*i_x = p0_x + (t * s1_x);
		if (i_y != NULL)
			*i_y = p0_y + (t * s1_y);
		return 1;
	}

	return 0;
}

int Object_collide(const struct Object *a, const struct Object *b)
{
	if (a->x == b->x && a->y == b->y) {
		return 1;
	}

	float x, y;

	return get_line_intersection(
	    a->x, a->y, a->x + a->direction_x, a->y + a->direction_y, b->x,
	    b->y, b->x + b->direction_x, b->y + b->direction_y, &x, &y);
}

void Object_input(struct Object *o, struct Object **rockets, int ch)
{
	switch (ch) {
	case 68:
		o->direction_y = 0;
		o->direction_x = -1;
		o->ch = "<";
		break;

	case 67:
		o->direction_y = 0;
		o->direction_x = 1;
		o->ch = ">";
		break;

	case 65:
		o->direction_y = -1;
		o->direction_x = 0;
		o->ch = "^";
		break;

	case 66:
		o->direction_y = 1;
		o->direction_x = 0;
		o->ch = "v";
		break;

	case ' ':
		for (int i = 0; i < MAXWEAPONS; i++) {
			if (!rockets[i]) {
				rockets[i] = Object_create(">", WEAPON);
				rockets[i]->direction_x = 2;
				rockets[i]->direction_y = 0;
				rockets[i]->x = o->x + 3;
				rockets[i]->y = o->y;
				return;
			}
		}
		break;

	default:
		o->direction_y = 0;
		o->direction_x = 0;
		o->ch = ">";
	}
}

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

void blue_array_clean(struct Object **array, uint32_t array_size)
{
	if (array) {
		for (uint32_t i = 0; i < array_size; i++) {
			if (array[i]) {
				free(array[i]);
			}
		}
	}
}

void blue_array_destroy(struct Object **array)
{
	if (array) {
		free(array);
	}
}

void blue_render_ship(WINDOW *field, struct Object *ship)
{
	mvwprintw(field, ship->y - 1, ship->x, "  |\\");
	mvwprintw(field, ship->y, ship->x, "<:||)");
	mvwprintw(field, ship->y + 1, ship->x, "  |/");
}

void blue_render_rock(WINDOW *field, struct Object *rock)
{
	mvwprintw(field, rock->y, rock->x, rock->ch);
}

int update_from_input()
{
	int c;
	char buf[1];
	int finished = 0;

	while (fread(buf, 1, 1, stdin) == 1) {
		c = buf[0];
		finished = c;
	}

	return finished;
}

void on_timer(int signum)
{
	// time_to_redraw = 1;
	// refresh();
}

int main()
{
	int ch = 0;
	int hits = 0;
	int i;
	int j;
	int max_x = 0;
	int max_y = 0;

	setlocale(LC_ALL, "");

	struct sigaction newhandler;      /* new settings         */
	sigset_t blocked;		  /* set of blocked sigs  */
	newhandler.sa_flags = SA_RESTART; /* options     */
	sigemptyset(&blocked);		  /* clear all bits       */
	newhandler.sa_mask = blocked;     /* store blockmask      */
	newhandler.sa_handler = on_timer; /* handler function     */
	if (sigaction(SIGALRM, &newhandler, NULL) == -1)
		perror("sigaction");

	int fd_flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (fd_flags | O_NONBLOCK));

	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 10000;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 10000;
	setitimer(ITIMER_REAL, &it, NULL);

	srand((unsigned)time(NULL));

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	getmaxyx(stdscr, max_y, max_x);

	WINDOW *field = newwin(BLUE_SPACE_HEIGHT, max_x, 0, 0);
	WINDOW *score = newwin(BLUE_SCORE_HEIGHT, max_x, BLUE_SPACE_HEIGHT, 0);

	struct Object *ship = Object_create(">", SHIP);
	struct Object **rocks = blue_array_create(MAX);
	struct Object **rockets = blue_array_create(MAXWEAPONS);

	for (i = 0; i < MAX; i++) {
		rocks[i] = Object_create("░", ROCK);
		rocks[i]->x = rand() % max_x;
		rocks[i]->y = rand() % (BLUE_SPACE_HEIGHT - 2) + 1;
	}

	nodelay(stdscr, TRUE);

	while (ch != 'q') {
		napms(DELAY);

		wrefresh(field);
		wrefresh(score);

		// INPUT
		////ch = getch();
		ch = update_from_input();
		// flushinp();
		Object_input(ship, rockets, ch);

		// MOVE
		Object_move(ship, max_x, BLUE_SPACE_HEIGHT - 2);

		for (i = 0; i < MAX; i++) {
			Object_move(rocks[i], max_x, BLUE_SPACE_HEIGHT - 2);
		}

		// HIT
		for (i = 0; i < MAX; i++) {
			if (Object_collide(ship, rocks[i])) {
				hits++;
			}
		}

		for (i = 0; i < MAXWEAPONS; i++) {
			if (rockets[i]) {
				for (j = 0; j < MAX; j++) {
					if (Object_collide(rockets[i],
							   rocks[j])) {
						rockets[i]->x = -1;
						rockets[i]->y = -1;
						rockets[i]->direction_x = 0;
						rockets[i]->direction_y = 0;

						rocks[j]->x = -1;
						rocks[j]->y = -1;
						rocks[j]->direction_x = 0;
						rocks[j]->direction_y = 0;
					}
				}
			}
		}

		wborder(score, 0, 0, 0, 0, 0, 0, 0, 0);

		// RENDER
		wclear(field);

		wborder(field, 1, 1, 0, 0, 1, 1, 1, 1);

		mvwprintw(score, 1, 1, "hits: %d", hits);
		mvwprintw(score, 1, 20, "ship: %d %d", ship->x, ship->y);
		mvwprintw(score, 1, 40, "key: %d", ch);

		blue_render_ship(field, ship);

		for (i = 0; i < MAX; i++) {
			blue_render_rock(field, rocks[i]);
		}

		for (i = 0; i < MAXWEAPONS; i++) {
			if (rockets[i]) {
				Object_move(rockets[i], max_x,
					    BLUE_SPACE_HEIGHT);
				mvwprintw(field, rockets[i]->y, rockets[i]->x,
					  rockets[i]->ch);
			}
		}
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
