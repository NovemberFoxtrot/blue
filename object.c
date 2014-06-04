#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <stdint.h>
#include "object.h"

const char* ship_design = ""
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

	if (o->next_x >= max_x || o->next_x < 0) {
		o->direction_x *= -1;
	} else {
		o->x += o->direction_x;
	}

	if (o->next_y >= max_y || o->next_y < 0) {
		o->direction_y *= -1;
	} else {
		o->y += o->direction_y;
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
	case KEY_LEFT:
		o->direction_y = 0;
		o->direction_x = -1;
		o->ch = "<";
		break;

	case KEY_RIGHT:
		o->direction_y = 0;
		o->direction_x = 1;
		o->ch = ">";
		break;

	case KEY_UP:
		o->direction_y = -1;
		o->direction_x = 0;
		o->ch = "^";
		break;

	case KEY_DOWN:
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
				rockets[i]->x = o->x+3;
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
