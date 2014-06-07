/*
        ====   =     =   =  ====
  -/|   =   =  =     =   =  =
<===::> ====   =     =   =  ====
  -\|   =   =  =     =   =  =
        ====   ====   ===   ====
*/

#include <curses.h>
#include <fcntl.h>
#include <linux/kd.h>
#include <locale.h>
#include <math.h>
#include <ncurses.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "blue.h"

static struct termios tty_attr_old;
static int old_keyboard_mode;
static int time_to_redraw;

int setup_keyboard()
{
	struct termios tty_attr;
	int flags;

	/* make stdin non-blocking */
	flags = fcntl(0, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags);

	/* save old keyboard mode */
	if (ioctl(0, KDGKBMODE, &old_keyboard_mode) < 0) {
		return 0;
	}

	tcgetattr(0, &tty_attr_old);

	/* turn off buffering, echo and key processing */
	tty_attr = tty_attr_old;
	tty_attr.c_lflag &= ~(ICANON | ECHO | ISIG);
	tty_attr.c_iflag &= ~(ISTRIP | INLCR | ICRNL | IGNCR | IXON | IXOFF);
	tcsetattr(0, TCSANOW, &tty_attr);

	ioctl(0, KDSKBMODE, K_MEDIUMRAW);

	return 1;
}

void restore_keyboard()
{
	tcsetattr(0, TCSAFLUSH, &tty_attr_old);
	ioctl(0, KDSKBMODE, old_keyboard_mode);
}

struct blue_object *blue_object_create(char *ch, enum blue_type type)
{
	struct blue_object *o = malloc(sizeof(struct blue_object));

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

void blue_object_move(struct blue_object *o, int max_x, int max_y)
{
	o->next_x = o->x + o->direction_x;
	o->next_y = o->y + o->direction_y;

	switch (o->type) {
	case WEAPON:
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
		break;

	case BACKGROUND:
		if (o->next_x < 2) {
			o->x = max_x;
			o->y = (rand() % (max_y - 2)) + 1;
		} else {
			o->x += o->direction_x;
		}
		break;

	default:
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

int blue_object_collide(const struct blue_object *a, const struct blue_object *b)
{
	if (a->x == b->x && a->y == b->y) {
		return 1;
	}

	float x, y;

	return get_line_intersection(
	    a->x, a->y, a->x + a->direction_x, a->y + a->direction_y, b->x,
	    b->y, b->x + b->direction_x, b->y + b->direction_y, &x, &y);
}

void blue_object_input(struct blue_object *o, struct blue_object **rockets, int ch)
{
	switch (ch) {
	case 68:
		o->direction_y = 0;
		o->direction_x = -1;
		break;

	case 67:
		o->direction_y = 0;
		o->direction_x = 1;
		break;

	case 65:
		o->direction_y = -1;
		o->direction_x = 0;
		break;

	case 66:
		o->direction_y = 1;
		o->direction_x = 0;
		break;

	case ' ':
		for (int i = 0; i < MAXWEAPONS; i++) {
			if (!rockets[i]) {
				rockets[i] = blue_object_create("~~~", WEAPON);
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

struct blue_object **blue_array_create(uint32_t array_size)
{
	struct blue_object **array;

	array = malloc(array_size * sizeof(struct blue_object));

	if (!array) {
		printf("malloc error");
		exit(1);
	}

	return array;
}

void blue_array_clean(struct blue_object **array, uint32_t array_size)
{
	if (array) {
		for (uint32_t i = 0; i < array_size; i++) {
			if (array[i]) {
				free(array[i]);
			}
		}
	}
}

void blue_array_destroy(struct blue_object **array)
{
	if (array) {
		free(array);
	}
}

void blue_render_ship(WINDOW *field, struct blue_object *ship)
{
	mvwprintw(field, ship->y - 1, ship->x, "  |\\");
	mvwprintw(field, ship->y, ship->x, "<:||)");
	mvwprintw(field, ship->y + 1, ship->x, "  |/");
}

void blue_render_rock(WINDOW *field, struct blue_object *rock)
{
	mvwprintw(field, rock->y, rock->x, rock->ch);
}

int update_from_input()
{
	int c = 0;
	char buf[1];

	while (fread(buf, 1, 1, stdin) == 1) {
		c = buf[0];
	}

	return c;
}

void on_timer(int signum)
{
	time_to_redraw = 1;
}

int main()
{
	int ch = 0;
	int hits = 0;
	int i;
	int j;
	int max_x = 0;
	int max_y = 0;

	time_to_redraw = 0;

	setlocale(LC_ALL, "");

	struct sigaction newhandler;      /* new settings         */
	sigset_t blocked;		  /* set of blocked sigs  */
	newhandler.sa_flags = SA_RESTART; /* options     */
	sigemptyset(&blocked);		  /* clear all bits       */
	newhandler.sa_mask = blocked;     /* store blockmask      */
	newhandler.sa_handler = on_timer; /* handler function     */
	if (sigaction(SIGALRM, &newhandler, NULL) == -1) {
		perror("sigaction");
	}

	setup_keyboard();

	struct itimerval it;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 40000;
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 40000;
	setitimer(ITIMER_REAL, &it, NULL);

	srand((unsigned)time(NULL));

	initscr();

	curs_set(0);

	getmaxyx(stdscr, max_y, max_x);

	WINDOW *field = newwin(BLUE_SPACE_HEIGHT, max_x, 0, 0);
	WINDOW *score = newwin(BLUE_SCORE_HEIGHT, max_x, BLUE_SPACE_HEIGHT, 0);

	struct blue_object *ship = blue_object_create(">", SHIP);
	struct blue_object **rocks = blue_array_create(MAX);
	struct blue_object **rockets = blue_array_create(MAXWEAPONS);
	struct blue_object **stars = blue_array_create(MAX);

	for (i = 0; i < MAX; i++) {
		rocks[i] = blue_object_create("*", BACKGROUND);
		rocks[i]->x = rand() % (max_x * 10);
		rocks[i]->y = rand() % (BLUE_SPACE_HEIGHT - 2) + 1;
		rocks[i]->direction_x = ((rand() % 4) + 2) * -1;
		rocks[i]->direction_y = 0;	

		stars[i] = blue_object_create(".", BACKGROUND);
		stars[i]->x = rand() % max_x;
		stars[i]->y = rand() % (BLUE_SPACE_HEIGHT - 2) + 1;
		stars[i]->direction_x = ((rand() % 2) + 1) * -1;
		stars[i]->direction_y = 0;
	}

	nodelay(stdscr, TRUE);

	while (ch != 'q') {
		if (time_to_redraw) {
			wrefresh(field);
			wrefresh(score);

			// INPUT
			ch = update_from_input();

			blue_object_input(ship, rockets, ch);

			// MOVE
			blue_object_move(ship, max_x, BLUE_SPACE_HEIGHT - 2);

			for (i = 0; i < MAX; i++) {
				blue_object_move(stars[i], max_x, BLUE_SPACE_HEIGHT - 2);
				blue_object_move(rocks[i], max_x, BLUE_SPACE_HEIGHT - 2);
			}

			for (i = 0; i < MAXWEAPONS; i++) {
				if (rockets[i]) {
					blue_object_move(rockets[i], max_x,
							 BLUE_SPACE_HEIGHT);
				}
			}

			// HIT
			for (i = 0; i < MAX; i++) {
				if (blue_object_collide(ship, rocks[i])) {
					hits++;
				}
			}

			for (i = 0; i < MAXWEAPONS; i++) {
				if (rockets[i]) {
					for (j = 0; j < MAX; j++) {
						if (blue_object_collide(
							rockets[i], rocks[j])) {
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
				time_to_redraw = 0;
			}
		}

		wborder(score, 0, 0, 0, 0, 0, 0, 0, 0);

		// RENDER
		wclear(field);

		wborder(field, 1, 1, 0, 0, 1, 1, 1, 1);

		mvwprintw(score, 1, 1, "hits: %d", hits);
		mvwprintw(score, 1, 20, "ship: %d %d", ship->x, ship->y);
		mvwprintw(score, 1, 40, "key: %#08x", ch);

		for (i = 0; i < MAX; i++) {
			blue_render_rock(field, stars[i]);
			blue_render_rock(field, rocks[i]);
		}

		for (i = 0; i < MAXWEAPONS; i++) {
			if (rockets[i]) {
				mvwprintw(field, rockets[i]->y, rockets[i]->x, rockets[i]->ch);
			}
		}

		blue_render_ship(field, ship);
	}

	blue_array_clean(rocks, MAX);
	blue_array_destroy(rocks);

	blue_array_clean(stars, MAX);
	blue_array_destroy(stars);

	blue_array_clean(rockets, MAX);
	blue_array_destroy(rockets);

	if (ship) {
		free(ship);
	}

	delwin(field);
	delwin(score);

	endwin();

	// restore_keyboard();
}
