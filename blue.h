#ifndef blue_h
#define blue_h

#define DELAY 50
#define MAX 20
#define BLUE_SPACE_HEIGHT 11
#define BLUE_SCORE_HEIGHT 3

enum blue_type { SHIP, WEAPON, ROCK, ALIEN, PLANET, BACKGROUND };
enum blue_status { ACTIVE, INACTIVE, EXPLODE, DESTROYED };

typedef void (*movement_handler)(int max_x, int max_y);

struct blue_object {
	enum blue_type type;
	enum blue_status status;

	int x;
	int y;

	int next_x;
	int next_y;

	int direction_x;
	int direction_y;

	int dimension_x;
	int dimension_y;

	int hits;

	movement_handler movement;	

	char *ch;
};

int setup_keyboard();
void restore_keyboard();
int update_from_input();

struct blue_object *blue_object_create(char *ch, enum blue_type type);
void blue_object_move(struct blue_object *o, int max_x, int max_y);
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);
int blue_object_collide(const struct blue_object *a, const struct blue_object *b);
void blue_object_input(struct blue_object *o, struct blue_object **rockets, int ch);
struct blue_object **blue_array_create(uint32_t array_size);
void blue_array_clean(struct blue_object **array, uint32_t array_size);
void blue_array_destroy(struct blue_object **array);
void blue_render_ship(WINDOW *field, struct blue_object *ship);
void blue_render_rock(WINDOW *field, struct blue_object *rock);
void on_timer(int signum);

#endif
