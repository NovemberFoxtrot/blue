#ifndef _object_h
#define _object_h

enum ObjectType {SHIP = 0, WEAPON, ROCK, ALIEN, PLANET};

struct Object
{
	enum ObjectType type;

	int x;
	int y;

	int next_x;
	int next_y;

	int direction_x;
	int direction_y;

	int hits;

	char *ch;
};


struct Object *Object_create(char *ch, enum ObjectType type);
void Object_move(struct Object *o, int max_x, int max_y);
int Object_collide(const struct Object *a, const struct Object *b);
void Object_input(struct Object *o, struct Object **rockets, int ch);

#endif
