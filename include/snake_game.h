#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position body[100]; 
    int length;
} Snake;

typedef struct {
    int width, height;
    int score;
    Position fruit;
    Snake snake;
    int direction;
} GameState;

typedef struct {
    int width, height;
    int score;
    Position fruit;
    int snake_length;
    Position snake_body[100]; 
} GameStateData;

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

void init_game(GameState *state, int height);
void render_game_from_data(GameStateData *data);

#endif

