#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#define MAX_SNAKE_LENGTH 200
#define MAX_OBSTACLES 200

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position body[MAX_SNAKE_LENGTH];
    int length;
} Snake;

typedef struct {
    int width;
    int height;
    int score;
    Position fruit;
    Snake snake;
    int direction;
    int time_remaining;
    int obstacles_flag;
    int obstacles_count;
    Position obstacles[MAX_OBSTACLES];
    int game_mode;
    int paused;
    int client_active_timer;
    int dead;
} GameState;

typedef struct {
    int width;
    int height;
    int score;
    Position fruit;
    Position snake_body[MAX_SNAKE_LENGTH];
    int snake_length;
    int time_remaining;
    int obstacles_flag;
    int obstacles_count;
    Position obstacles[MAX_OBSTACLES];
    int game_mode;
    int paused;
    int dead;
} GameStateData;

void init_game(GameState *state, int height);
void render_game_from_data(GameStateData *data);

#endif

