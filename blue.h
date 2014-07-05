#ifndef blue_h
#define blue_h

#define DELAY 45
#define MAX 100
#define BLUE_SPACE_HEIGHT 11
#define BLUE_SCORE_HEIGHT 3

enum blue_type { SHIP, WEAPON, ROCK, ALIEN, PLANET, BACKGROUND };
enum blue_object_status { ACTIVE, INACTIVE, EXPLODE, DESTROYED };
enum blue_game_status { RUN, STOP };

struct blue_game_state {
	WINDOW *field;
	WINDOW *score;

	enum blue_game_status status;

	int max_x;
	int max_y;
	int ch;
};

struct blue_object;

typedef void (*movement_handler)(struct blue_object *o, struct blue_game_state *game_state);
typedef void (*render_handler)(struct blue_object *o, struct blue_game_state *game_state);

struct blue_object {
	enum blue_type type;
	enum blue_object_status status;

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
	render_handler render;	

	char *ch;
};


char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);
int blue_game_setup_keyboard(void);
int blue_object_collide(const struct blue_object *a, const struct blue_object *b);
int update_from_input(void);
struct blue_game_state *blue_game_state_create(void);
struct blue_object **blue_array_create(uint32_t array_size);
struct blue_object **blue_game_create_objects(struct blue_game_state *game_state);
struct blue_object *blue_object_create(char *ch, enum blue_type type);
void blue_array_clean(struct blue_object **array, uint32_t array_size);
void blue_array_destroy(struct blue_object **array);
void blue_game_init(struct blue_game_state *game_state);
void blue_game_run(struct blue_game_state *game_state, struct blue_object **objects);
void blue_object_background_movement_handler(struct blue_object *o, int max_x, int max_y);
void blue_object_input(struct blue_object *o, struct blue_object **rockets, int ch);
void blue_object_move(struct blue_object *o, int max_x, int max_y);
void blue_render_handler_laser(struct blue_object *o, struct blue_game_state *game_state);
void blue_render_handler_rock(struct blue_object *o, struct blue_game_state *game_state);
void blue_render_handler_ship(struct blue_object *o, struct blue_game_state *game_state);
void blue_render_rock(WINDOW *field, struct blue_object *rock);
void blue_render_ship(WINDOW *field, struct blue_object *ship);
void on_timer(int signum);
void restore_keyboard(void);

#endif
