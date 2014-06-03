#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include <stdint.h>
#include "object.h"

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

int Object_collide(const struct Object *a, const struct Object *b)
{
	if (a->x == b->x && a->y == b->y) {
		return 1;
	}

	return 0;
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
		if (!rockets[0]) {
			rockets[0] = Object_create("=", WEAPON);
			rockets[0]->direction_x = o->direction_x * 2;
			rockets[0]->direction_y = o->direction_y * 2;
			rockets[0]->x = o->x;
			rockets[0]->y = o->y;
		}
		break;
	}
}
