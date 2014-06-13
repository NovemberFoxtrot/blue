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

	flags = fcntl(0, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags);

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

void blue_object_background_movement_handler(struct blue_object *o, int max_x, int max_y)
{
	o->next_x = o->x + o->direction_x;
	o->next_y = o->y + o->direction_y;

	if (o->next_x < 2) {
		o->x = max_x;
		o->y = (rand() % (max_y - 2)) + 1;
	} else {
		o->x += o->direction_x;
	}
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
			o->y = (rand() % (BLUE_SPACE_HEIGHT - 2)) + 1;
			o->direction_x = ((rand() % 6) + 1) * -1;
			o->ch = o->direction_x < -2 ? "*" : ".";
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

char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
	float s1_x, s1_y, s2_x, s2_y;

	s1_x = p1_x - p0_x;
	s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;
	s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
		if (i_x != NULL) {
			*i_x = p0_x + (t * s1_x);
		}
		if (i_y != NULL) {
			*i_y = p0_y + (t * s1_y);
		}

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

	return get_line_intersection(a->x, a->y, a->x + a->direction_x, a->y + a->direction_y, b->x, b->y, b->x + b->direction_x, b->y + b->direction_y, &x, &y);
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
		for (int i = MAX; i < (MAX*2); i++) {
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
	if(rock->direction_x >= -4) {
		rock->ch = ".";
	} else {
		rock->ch = "*";
	}

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

	printf("%d", signum);
}

struct blue_game_state *blue_game_state_create() {
	struct blue_game_state *game_state;

	game_state = calloc(1, sizeof(struct blue_game_state));

	if(game_state == NULL) {
		printf("malloc error: game state\n");
		exit(1);
	}

	return game_state;
}

void blue_game_init(struct blue_game_state *game_state) {
	game_state->ch = 0;
	game_state->status = RUN;

	initscr();

	getmaxyx(stdscr, game_state->max_y, game_state->max_x);
	
	game_state->field = newwin(BLUE_SPACE_HEIGHT, game_state->max_x, 0, 0);
	game_state->score = newwin(BLUE_SCORE_HEIGHT, game_state->max_x, BLUE_SPACE_HEIGHT, 0);

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

	curs_set(0);

	nodelay(stdscr, TRUE);
}

struct blue_object **blue_game_create_objects(struct blue_game_state *game_state) {
	struct blue_object **objects = blue_array_create(MAX*2);

	objects[0] = blue_object_create(">", SHIP);
	objects[0]->x = game_state->max_x / 4;
	objects[0]->y = BLUE_SPACE_HEIGHT / 2;

	for (int i = 1; i < MAX; i++) {
		objects[i] = blue_object_create(".", BACKGROUND);
		objects[i]->x = rand() % game_state->max_x;
		objects[i]->y = (rand() % (BLUE_SPACE_HEIGHT - 2)) + 1;
		objects[i]->direction_x = ((rand() % 5) + 1) * -1;
		objects[i]->direction_y = 0;	
	}

	return objects;
}

void blue_game_run(struct blue_game_state *game_state, struct blue_object **objects) {
	while (game_state->status == RUN) {
		if (time_to_redraw) {
			wrefresh(game_state->field);
			wrefresh(game_state->score);

			// INPUT
			game_state->ch = update_from_input();

			if (game_state->ch == 'q') {
				game_state->status = STOP;
			}

			blue_object_input(objects[0], objects, game_state->ch);

			// MOVE
			for (int i = 0; i < MAX; i++) {
				if (objects[i]) {
					blue_object_move(objects[i], game_state->max_x, BLUE_SPACE_HEIGHT - 2);

					if (objects[i]->type != BACKGROUND && blue_object_collide(objects[0], objects[i])) {
					 	objects[0]->hits++;
					}
				}
			}

			wborder(game_state->score, 0, 0, 0, 0, 0, 0, 0, 0);

			// RENDER
			wclear(game_state->field);

			wborder(game_state->field, 1, 1, 0, 0, 1, 1, 1, 1);

			mvwprintw(game_state->score, 1, 1, "hits: %d", objects[0]->hits);
			mvwprintw(game_state->score, 1, 20, "ship: %d %d", objects[0]->x, objects[0]->y);
			mvwprintw(game_state->score, 1, 40, "key: %#08x", game_state->ch);

			for (int i = 1; i < MAX; i++) {
				blue_render_rock(game_state->field, objects[i]);
			}

			time_to_redraw = 0;
		}

		blue_render_ship(game_state->field, objects[0]);
	}
}

int main()
{
	struct blue_game_state *game_state = blue_game_state_create();

	blue_game_init(game_state);

	struct blue_object **objects = blue_game_create_objects(game_state);

	blue_game_run(game_state, objects);

	blue_array_clean(objects, MAX);
	blue_array_destroy(objects);

	if(game_state) {
		free(game_state);
	}

	delwin(game_state->field);
	delwin(game_state->score);

	endwin();
}
